#include "TrapezoidalEnv.h"
#include <cstdint>

namespace dsp {

void TrapezoidalEnv::init(const uint32_t fadeLength,
                          bool autoRetrigger = false) {
  _fadeLength = fadeLength;
  _autoRetriggered = autoRetrigger;
}

void TrapezoidalEnv::attack() {
  _state = EnvState::FADE_IN;
  _counter = 0U;
}

void TrapezoidalEnv::release() {
  if (_state != EnvState::FADE_OUT) {
    _state = EnvState::FADE_OUT;
    _counter = _fadeLength;
  }
}

bool TrapezoidalEnv::isActive() { return _state != EnvState::IDLE; }

float TrapezoidalEnv::advance() {
  float currentVal = 0.0;
  switch (_state) {
  case EnvState::IDLE:
    currentVal = 0.0;
    break;
  case EnvState::FADE_IN:
    _counter++;
    currentVal = getFromCounter();
    if (_counter == _fadeLength) {
      _state = EnvState::SUSTAIN;
    }
    break;
  case EnvState::SUSTAIN:
    currentVal = getFromCounter();
    break;
  case EnvState::FADE_OUT:
    _counter--;
    currentVal = getFromCounter();
    if (_counter == 0U) {
      if (_autoRetriggered) {
        attack();
      } else {
        _state = EnvState::IDLE;
      }
    }
    break;
  default:
    break;
  }
  return currentVal;
}

} // namespace dsp