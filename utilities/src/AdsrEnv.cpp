//
// Created by cesar on 8/13/22.
//

#include "AdsrEnv.h"
#include <cmath>
#include <cstring>

namespace utilities{

template <typename... Ts> // (7)
struct Overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

AdsrEnv::AdsrEnv() {
    _attackSection.overshoot = 0.3;
    _releaseSection.overshoot = 0.3;
    _decaySection.overshoot = 0.3;
}

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
  _stm.transition(NoteOnEvt());
}
void AdsrEnv::release() {
  _stm.transition(NoteOffEvt{});
}

void AdsrEnv::reset() {
  _envelopeValue = 0.0f;
}

/**
 * TODO: Think about when to set it to the target values in each step.
 * A_db = 20 log_10 (A_liner)
 * @return
 */
float AdsrEnv::tick() {
  auto visitor = Overloaded{[&](Idle) { _envelopeValue = 0.0f; },
                            [&](Attack) {
                                _envelopeValue = _attackSection.offset + _envelopeValue * _attackSection.multiplier;
                              if (_envelopeValue >= 1.0) {
                                  _envelopeValue = 1.0;
                                  _stm.transition(TargetLevelReached{});
                              }
                            },
                            [&](Decay) {
                                _envelopeValue = _decaySection.offset + _envelopeValue * _decaySection.multiplier;
                                if (_envelopeValue <= _parameters.sustainLevel) {
                                    _envelopeValue = _parameters.sustainLevel;
                                    _stm.transition(TargetLevelReached{});
                                }
                            },
                            [&](Sustain) {},
                            [&](Release) {
                                _envelopeValue = _releaseSection.offset + _envelopeValue * _releaseSection.multiplier;
                                if (_envelopeValue <= 0.0) {
                                    _envelopeValue = 0.0;
                                    _stm.transition(TargetLevelReached{});
                                }
                            }};
  std::visit(visitor, _stm.getState());
  return _envelopeValue;
}

void AdsrEnv::applyToBuffer(float *outBuffer, int numSamples){

    auto multVistor = Overloaded{[&](Idle) {AudioBufferTools::clearRawBuffer(outBuffer,numSamples*2);},
                              [&](Sustain){AudioBufferTools::multiplyRawBuffer(outBuffer,numSamples*2,_envelopeValue);},
                              [&](auto otherState) {
                                  for (int i = 0; i < numSamples; ++i) {
                                      *outBuffer++ *= tick();
                                  }
                              }};

    std::visit(multVistor,_stm.getState());
}


void AdsrEnv::updateRates() {
    _attackSection.multiplier = getRateMult(_attackSection.overshoot,1.0f + _attackSection.overshoot, static_cast<uint32_t>(_parameters.attackTimeSec*_sampleRate));
    _attackSection.offset = (1.f + _attackSection.overshoot) * (1.f - _attackSection.multiplier);
    _decaySection.multiplier = getRateMult(_decaySection.overshoot,1.0f + _decaySection.overshoot, static_cast<uint32_t>(_parameters.decayTimeSec*_sampleRate));
    _decaySection.offset = (_parameters.sustainLevel -_decaySection.overshoot) * (1.f - _decaySection.multiplier);
    _releaseSection.multiplier = getRateMult(_releaseSection.overshoot,1.0f + _releaseSection.overshoot, static_cast<uint32_t>(_parameters.releaseTimeSec*_sampleRate));
    _releaseSection.offset = (-_releaseSection.overshoot) * (1.f - _releaseSection.multiplier);
}

float AdsrEnv::getRateMult(float startLevel, float endLevel, uint32_t lengthInSamples) {
        return expf(-logf(endLevel/ startLevel) / static_cast<float>(lengthInSamples));
}

}
