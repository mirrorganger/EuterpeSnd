#pragma once

#include "Delay.hpp"

namespace dsp {

class AllPassFilter {
public:
  AllPassFilter(float sampleRate_hz, float delayMax_ms, float gain)
      : _g(gain), _sampleRate_hz(sampleRate_hz),
        _delay(delayMax_ms * sampleRate_hz / 1000.0F) {}

  void prepare(float sampleRate_hz, float delay_ms) {
    _sampleRate_hz = sampleRate_hz;
    _delay.clear();
    setDelay(delay_ms);
  }

  void setGain(float gain) { _g = gain; }

  void setDelay(float delay_ms) {
    _delayIndex = (delay_ms * _sampleRate_hz / 1000.F);
  }

  void setDelaySamples(float delaySamples) { _delayIndex = delaySamples; }

  float process(const float xn) {
    auto wn_D = _delay[_delayIndex];
    auto wn = xn + _g * wn_D;
    _delay.push(wn);
    return wn_D + -_g * wn;
  }

  float getSampleAtDelay(float delayInSamples) {
    return _delay[delayInSamples];
  }

private:
  float _g;
  float _delayIndex;
  float _sampleRate_hz;
  FractionalDelay _delay;
};

} // namespace dsp