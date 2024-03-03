#pragma once

#include "Delay.hpp"
#include "OnePoleLpf.h"

namespace dsp {

class CombFilter {
public:
  CombFilter(float sampleRate_hz, float delayMax_ms, float gain, bool lpfEnabled = true)
      : _g(gain), _sampleRate_hz(sampleRate_hz),
        _delay(delayMax_ms * sampleRate_hz / 1000.0F), _lpfEnabled(lpfEnabled){
          _lpf.setFeedbackGain(1.0F-gain);
        }

  void prepare(float sampleRate_hz, float delay_ms) {
    _sampleRate_hz = sampleRate_hz;
    _delay.clear();
    setDelay(delay_ms);
  }

  void setGain(float gain) { _g = gain; _lpf.setFeedbackGain(1.0F-gain); }

  void setDelay(float delay_ms) {
    _delayIndex = (delay_ms * _sampleRate_hz / 1000.0F);
  }

  float process(const float xn) {
    auto yn = _delay[_delayIndex];
    if(_lpfEnabled){
      yn = _lpf.process(yn);      
    }  
    auto delayed = xn + _g * yn;
    _delay.push(delayed);
    return yn;
  }

private:
  float _g;
  float _sampleRate_hz;
  float _delayIndex = 0.0F;
  FractionalDelay _delay;
  bool _lpfEnabled;
  OnePoleLpf _lpf;
};

} // namespace dsp