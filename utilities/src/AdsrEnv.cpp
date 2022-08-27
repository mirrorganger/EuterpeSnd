//
// Created by cesar on 8/13/22.
//

#include "AdsrEnv.h"
#include <cmath>

namespace utilities{

template <typename... Ts> // (7)
struct Overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

AdsrEnv::AdsrEnv() {}

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
bool AdsrEnv::isActive() const {
  return !std::holds_alternative<Idle>(_stm.getState());
}

void AdsrEnv::trigger() {
  _stm.transition(NoteOnEvt());
  _counter = static_cast<int>(_parameters.attackTimeSec * _sampleRate);
}
void AdsrEnv::release() {
  _stm.transition(NoteOffEvt{});
  _counter = static_cast<int>(_parameters.releaseTimeSec * _sampleRate);
}

void AdsrEnv::reset() {
  _counter = 0U;
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
                              if (_counter == 0) {
                                _stm.transition(CounterExpiredEvt{});
                                _counter = static_cast<uint64_t>(_parameters.decayTimeSec * _sampleRate);
                              } else {
                                _counter--;
                                _envelopeValue += _attackRate;
                              }
                            },
                            [&](Decay) {
                              if (_counter == 0) {
                                _stm.transition(CounterExpiredEvt{});
                              } else {
                                _counter--;
                                _envelopeValue -= _decayRate;
                              }
                            },
                            [&](Sustain) {},
                            [&](Release) {
                              if (_counter == 0) {
                                _stm.transition(CounterExpiredEvt{});
                              } else {
                                _counter--;
                                _envelopeValue -= _releaseRate;
                              }
                            }};
  std::visit(visitor, _stm.getState());
  return _envelopeValue;
}

void AdsrEnv::updateRates() {
  _attackRate = 1.0f / (_parameters.attackTimeSec * _sampleRate);
  _decayRate = (1.0f - _parameters.sustainLevel) /
               (_parameters.decayTimeSec * _sampleRate);
  _releaseRate =
      (_parameters.sustainLevel) / (_parameters.releaseTimeSec * _sampleRate);
}
    // TODO: Check this with python
    void AdsrEnv::updateCoeff(float startLevel, float endLevel, uint32_t lengthInSamples) {
        _multCoeff = 1.0 + (log(static_cast<double>(endLevel)) - static_cast<double>(log(startLevel))) / (lengthInSamples);
    }

}
