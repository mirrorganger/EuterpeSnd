#include "WavetableOsc.h"

#include <AudioBufferTools.h>
#include <algorithm>
#include <cmath>
#include <string.h>

namespace synthtools {

WavetableOsc::WavetableOsc(
    const utilities::AudioBufferTools::OscillatorType oscillatorType,
    const uint32_t tableSize, const float sampleRate,
    const bool useInterpolation) {
  setUp(oscillatorType, tableSize, sampleRate, useInterpolation);
}

void WavetableOsc::setUp(
    const utilities::AudioBufferTools::OscillatorType oscillatorType,
    const uint32_t tableSize, const float sampleRate,
    const bool useInterpolation) {
  _inverseSampleRate = 1.0f / sampleRate;
  _useInterpolation = useInterpolation;
  _readPointer = 0.0f;
  _buffer.resize(tableSize);
  utilities::AudioBufferTools::makeOscTable(_buffer, oscillatorType);
}

void WavetableOsc::setUp(const uint32_t tableSize, const float sampleRate,
                         BuilderFunc builder, const bool useInterpolation) {
  _inverseSampleRate = 1.0f / sampleRate;
  _useInterpolation = useInterpolation;
  _readPointer = 0;
  _buffer.resize(tableSize);
  builder(_buffer);
}

void WavetableOsc::setFrequency(const float f) {
  _freq = f;
  updateIncrement();
}

void WavetableOsc::setAmplitude(const float amplitude) {
  _amplitude = amplitude;
}

float WavetableOsc::process() {
  float outValue = 0.0;

  if (_buffer.size() == 0)
    return outValue;

  if (_useInterpolation)
    outValue =
        utilities::AudioBufferTools::interpolation(_buffer, _readPointer);
  else
    outValue = _buffer[(int)_readPointer];

  outValue *= _amplitude;
  _readPointer += _tableIncrement.load();
  while (static_cast<int>(_readPointer) >= _buffer.size())
    _readPointer -= static_cast<float>(_buffer.size());

  return outValue;
}

void WavetableOsc::process(utilities::AudioBuffer<float> &buffer) {

  if (_buffer.size() == 0)
    return;

  auto wrPtr = buffer.getWritePointer();

  for (uint32_t sample_i = 0; sample_i < buffer.getFramesPerBuffer();
       ++sample_i) {
    auto sample = process();
    for (size_t channel_i = 0; channel_i < buffer.getNumberOfChannels();
         channel_i++) {
      *wrPtr++ = sample;
    }
  }
}

void WavetableOsc::updateIncrement() {
  _tableIncrement.store(static_cast<float>(_buffer.size()) * _freq *
                        _inverseSampleRate);
}

} // namespace synthtools
