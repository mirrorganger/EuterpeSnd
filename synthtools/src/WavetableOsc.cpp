#include "WavetableOsc.h"

#include <algorithm>
#include <cmath>
#include <string.h>
#include <AudioBufferTools.h>

namespace synthtools {

    WavetableOsc::WavetableOsc(const utilities::AudioBufferTools::OscillatorType oscillatorType,
                               const uint32_t tableSize, const float sampleRate,
                               const bool useInterpolation) {
        setUp(oscillatorType, tableSize, sampleRate, useInterpolation);
    }

    void WavetableOsc::setUp(const utilities::AudioBufferTools::OscillatorType oscillatorType,
                             const uint32_t tableSize, const float sampleRate,
                             const bool useInterpolation) {
        _inverseSampleRate = 1.0f / sampleRate;
        _useInterpolation = useInterpolation;
        _readPointer = 0.0f;
        _buffer.resize(tableSize);
        utilities::AudioBufferTools::makeOscTable(_buffer,oscillatorType);
    }

    void WavetableOsc::setUp(const uint32_t tableSize, const float sampleRate, BuilderFunc builder,
                             const bool useInterpolation) {
        _inverseSampleRate = 1.0f / sampleRate;
        _useInterpolation = useInterpolation;
        _readPointer = 0;
        _buffer.resize(tableSize);
        _oscillatorType = utilities::AudioBufferTools::OscillatorType::FREE;
        builder(_buffer);
    }

    void WavetableOsc::setFrequency(const float f) {
        _freq = f;
        updateIncrement();
    }

    float WavetableOsc::getFrequency() const { return _freq; }

    void WavetableOsc::setAmplitude(const float amplitude) {
        _amplitude = amplitude;
    }

    float WavetableOsc::process() {
        float outValue = 0.0;

        if (_buffer.size() == 0)
            return outValue;

        if (_useInterpolation)
            outValue = utilities::AudioBufferTools::interpolation(_buffer, _readPointer);
        else
            outValue = _buffer[(int) _readPointer];

        outValue *= _amplitude;
        _readPointer += _tableIncrement.load();

        return outValue;
    }

    void WavetableOsc::renderAudio(float *audioData, uint32_t numFrames) {
        if (_buffer.size() == 0)
            return;

        for (uint32_t i = 0; i < numFrames; ++i) {
            audioData[i] = process();
        }
    }

    void WavetableOsc::updateIncrement() {
        _tableIncrement.store(static_cast<float>(_buffer.size()) * _freq * _inverseSampleRate);
    }

}
