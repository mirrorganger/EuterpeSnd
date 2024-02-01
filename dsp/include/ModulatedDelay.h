#pragma once

#include "AudioBuffer.h"
#include "DelayLine.h"
#include "Lfo.h"

namespace dsp {

template <typename DataType, std::size_t MAX_DELAY> class ModulatedDelay {
public:
  ModulatedDelay(float sampleRate_hz, float lfoFreq_hz, float delay_ms,
                 float lfoDepth, float feedback)
      : _lfo(sampleRate_hz, lfoFreq_hz, Lfo::WaveFormT::TRIANGLE),
        _sampleRate_hz(sampleRate_hz), _baseDelay(_sampleRate_hz * delay_ms / 1000.0), _lfoDepth(lfoDepth),_feedback(feedback) {
            _delayLine.clear();
            _delayLine.setDelay(_baseDelay);
        }

  DataType process(DataType in) {
    auto out = DataType{};
    auto lfoOut = static_cast<DataType>(_lfo.getNextSample());
    _delayLine.setDelay(static_cast<float>(_baseDelay  +  lfoOut * _lfoDepth));
    DataType yn = _delayLine.read();
    DataType dn = in + _feedback * yn;
    _delayLine.write(dn);
    out = _dry * in + _wet * yn;
    return out;
  }

  void process(utilities::AudioBuffer<DataType> &buffer) {
    auto wrtPtr = buffer.getWritePointer();
    for (int sample_i = 0; sample_i < buffer.getFramesPerBuffer(); ++sample_i) {
      auto processed = process(*wrtPtr);
      for (int channel_i = 0; channel_i < buffer.getNumberOfChannels();
           ++channel_i) {
        (*wrtPtr) = processed;
        wrtPtr++;
      }
    }
  }

private:
  DelayLine<DataType, MAX_DELAY> _delayLine;
  Lfo _lfo;
  float _sampleRate_hz;
  float _baseDelay;
  float _lfoDepth = 1.0F;
  float _feedback = 0.5F;
  float _wet = 0.9F;
  float _dry = 0.1F;
};

} // namespace dsp
