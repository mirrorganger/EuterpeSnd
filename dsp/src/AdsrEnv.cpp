//
// Created by cesar on 8/13/22.
//

#include "AdsrEnv.h"
#include <cmath>
#include <cstring>

namespace dsp{

template <typename... Ts> // (7)
struct Overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

void AdsrEnv::configure(const AdsrEnv::Parameters &param) {
  _parameters = param;
}

void AdsrEnv::setSampleRate(float sampleRate) {
  _sampleRate = sampleRate;
  updateRates();
}
void AdsrEnv::setAttackTime(float attackTimeSec) {
  _parameters.attackTimeSec = attackTimeSec;
  updateRates();
}
void AdsrEnv::setReleaseTime(float releaseTimeSec) {
  _parameters.releaseTimeSec = releaseTimeSec;
  updateRates();
}

void AdsrEnv::setDecayTime(float decayTimeSec) {
    _parameters.decayTimeSec = decayTimeSec;
    updateRates();
}

bool AdsrEnv::isActive() const {
  return !std::holds_alternative<Idle>(_stm.getState());
}

void AdsrEnv::trigger() {
    _envelopeValue = _minimumLevel;
    _currentSectionSample = 0U;
  _stm.transition(NoteOnEvt());
}
void AdsrEnv::release() {
    _currentSectionSample = 0U;
  _stm.transition(NoteOffEvt{});
}

void AdsrEnv::reset() {
  _envelopeValue = _minimumLevel;
}

/**
 * TODO: Think about when to set it to the target values in each step.
 * A_db = 20 log_10 (A_liner)
 * @return
 */
float AdsrEnv::tick() {
  auto visitor = Overloaded{[&](Idle) { _envelopeValue = 0.0f;},
                            [&](Attack) {
                                _envelopeValue *= _attackSection.advanceMultiplier;
                                _currentSectionSample++;
                                if (_currentSectionSample ==  _attackSection.length) {
                                  _envelopeValue = 1.0;
                                  _currentSectionSample = 0U;
                                  _stm.transition(TargetLevelReached{});
                              }
                            },
                            [&](Decay) {
                                _envelopeValue *= _decaySection.advanceMultiplier;
                                _currentSectionSample++;
                                if (_currentSectionSample ==  _decaySection.length) {
                                    _envelopeValue = _parameters.sustainLevel;
                                    _currentSectionSample = 0U;
                                    _stm.transition(TargetLevelReached{});
                                }
                            },
                            [&](Sustain) {},
                            [&](Release) {
                                _envelopeValue *= _releaseSection.advanceMultiplier;
                                _currentSectionSample++;
                                if (_currentSectionSample == _releaseSection.length) {
                                    _currentSectionSample = 0U;
                                    //_envelopeValue = _minimumLevel;
                                    _stm.transition(TargetLevelReached{});
                                }
                            }};
  std::visit(visitor, _stm.getState());
  return static_cast<float>(_envelopeValue);
}

void AdsrEnv::process(utilities::AudioBuffer<float> &buffer) {
    auto multVisitor = Overloaded{[&](Idle) {utilities::AudioBufferTools::clearRawBuffer(buffer.getWritePointer(),buffer.getNumberOfChannels()*buffer.getFramesPerBuffer());},
                                 [&](Sustain){utilities::AudioBufferTools::multiplyRawBuffer(buffer.getWritePointer(),buffer.getNumberOfChannels()*buffer.getFramesPerBuffer(),_envelopeValue);},
                                 [&](auto otherState) {
                                     auto wrtPtr = buffer.getWritePointer();
                                     for (int sample_i = 0; sample_i < buffer.getFramesPerBuffer(); ++sample_i) {
                                         auto multCached = tick();
                                         for (int channel_i = 0; channel_i < buffer.getNumberOfChannels() ; ++channel_i) {
                                             (*wrtPtr) *= multCached;
                                             wrtPtr++;
                                         }
                                     }
                                 }};

    std::visit(multVisitor,_stm.getState());
}


void AdsrEnv::updateRates() {
    _attackSection.length = static_cast<uint64_t>(_parameters.attackTimeSec * _sampleRate);
    _attackSection.advanceMultiplier = getRateMult(_minimumLevel,1.0, _parameters.attackTimeSec);
    _decaySection.length = static_cast<uint64_t>(_parameters.decayTimeSec * _sampleRate);
    _decaySection.advanceMultiplier  = getRateMult(1.0,_parameters.sustainLevel,_parameters.decayTimeSec);
    _releaseSection.length = static_cast<uint64_t>(_parameters.releaseTimeSec * _sampleRate);
    _releaseSection.advanceMultiplier = getRateMult(_parameters.sustainLevel,_minimumLevel,_parameters.releaseTimeSec);
}
double AdsrEnv::getRateMult(double startLevel, double endLevel, double intervalTimeSec) {
        return 1.0 + (log(endLevel) - log(startLevel)) / (intervalTimeSec * _sampleRate);
}


}
