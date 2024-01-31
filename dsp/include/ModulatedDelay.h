#pragma once

#include "DelayLine.h"
#include "Lfo.h"

namespace dsp {

template <typename DataType, std::size_t MAX_DELAY> class ModulatedDelay {
public:
  ModulatedDelay(float sampleRate_hz, float lfoFreq_hz, float lfoDepth,
                 float feedback, float delay_ms)
      _lfo(sampleRate_hz, lfoFreq_hz, 0.6, Lfo::WaveFormT::TRIANGLE)
          _sampleRate_hz(sampleRate_hz),
      _baseDelay_ms(_baseDelay_ms) {}

  DataType process(DataType in) {
    auto out = DataType{};
    auto lfoOut =
        static_cast<DataType>(lfo.getNextSample()) auto dealayInSamples =
            _sampleRate_hz * lfoOut * _lfoDepth;
    _delayLine.setDelay(static_cast<float>(delayInSamples));
    DataType yn = _delayLine.read();
    DataType dn = in + _feedback * yn;
    _delayLine.write(dn);
    double out = _dry * xn + _wet * yn;
    return out;
  }

private:
  DelayLine<DataType, MAX_DELAY> _delayLine;
  Lfo _lfo;
  float _sampleRate_hz;
  float _baseDelay_ms;
  float _lfoDepth = 1.0F;
  float _feedback = 0.5F;
  float _wet = 0.5F;
  float _dry = 0.5F;
};

} // namespace dsp
