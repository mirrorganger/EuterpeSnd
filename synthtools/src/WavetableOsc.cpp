#include "WavetableOsc.h"

#include <algorithm>
#include <cmath>
#include <string.h>

namespace synthtools {

    constexpr uint8_t HIGHER_HARMONIC = 48;

    float interpolation(const std::vector<float> &buffer, float indexPtr) {

        int idxBelow = floorf(indexPtr);
        int idxAbove = idxBelow + 1;
        if (idxAbove >= buffer.size())
            idxAbove = 0;

        float weigthAbove = indexPtr - idxBelow;

        return (1.0 - weigthAbove) * buffer[idxBelow] +
               weigthAbove * buffer[idxAbove];
    }

    WavetableOsc::WavetableOsc(const OscillatorType oscillatorType,
                               const uint32_t tableSize, const float sampleRate,
                               const bool useInterpolation) {
        setUp(oscillatorType, tableSize, sampleRate, useInterpolation);
    }

    void WavetableOsc::setUp(const OscillatorType oscillatorType,
                             const uint32_t tableSize, const float sampleRate,
                             const bool useInterpolation) {
        _inverseSampleRate = 1.0 / sampleRate;
        _useInterpolation = useInterpolation;
        _readPointer = 0;
        _buffer.resize(tableSize);
        switch (oscillatorType) {
            case OscillatorType::SINE:
                makeSineOscillator();
                break;
            case OscillatorType::TRIANGLE:
                makeTriangleOscillator();
                break;
            case OscillatorType::SQUARE:
                makeSquareOscillator();
                break;
            case OscillatorType::SAWTOOTH:
                makeSawToothOscillator();
                break;
        }
    }

    void WavetableOsc::setUp(const uint32_t tableSize, const float sampleRate, BuilderFunc builder,
                             const bool useInterpolation) {
        _inverseSampleRate = 1.0 / sampleRate;
        _useInterpolation = useInterpolation;
        _readPointer = 0;
        _buffer.resize(tableSize);
        _oscillatorType = OscillatorType::FREE;
        builder(_buffer);
    }

    void WavetableOsc::setFrequency(const float f) {
        _freq = f;
        updateIncrement();
    }

    float WavetableOsc::getFrequency() const { return _freq; }

    float WavetableOsc::process() {

        float outValue = 0.0;

        if (_buffer.size() == 0)
            return outValue;

        if (_useInterpolation)
            outValue = interpolation(_buffer, _readPointer);
        else
            outValue = _buffer[(int) _readPointer];

        _readPointer += _tableIncrement;

        return outValue;
    }

    void WavetableOsc::process(float *bufferToFill, uint32_t numFrames) {

        memset(bufferToFill, 0, sizeof(float) * numFrames);

        if (_buffer.size() == 0)
            return;

        for (uint32_t i = 0; i < numFrames; ++i) {
            bufferToFill[i] = process();
        }
    }

    void WavetableOsc::updateIncrement() {
        _tableIncrement = _buffer.size() * _freq * _inverseSampleRate;
    }

    void WavetableOsc::makeSineOscillator() {
        std::generate(_buffer.begin(), _buffer.end(), [&, n = 0]() mutable {
            return sinf(2.0 * M_PI * static_cast<float>(n++) /
                        static_cast<float>(_buffer.size()));
        });
    }

    void WavetableOsc::makeTriangleOscillator() {
        std::generate(_buffer.begin(), _buffer.begin() + _buffer.size() / 2,
                      [&, n = 0]() mutable {
                          return -1.0 + 4.0 * static_cast<float>(n++) /
                                        static_cast<float>(_buffer.size());
                      });
        std::generate(_buffer.begin() + _buffer.size() / 2, _buffer.end(),
                      [&, n = _buffer.size() / 2]() mutable {
                          return 1.0 -
                                 4.0 * static_cast<float>(n++ - _buffer.size() / 2) /
                                 static_cast<float>(_buffer.size());
                      });
    }

    void WavetableOsc::makeSquareOscillator() {
        std::fill(_buffer.begin(), _buffer.begin() + _buffer.size() / 2, 1.0);
        std::fill(_buffer.begin() + _buffer.size() / 2, _buffer.end(), -1.0);
    }

    void WavetableOsc::makeSawToothOscillator() {
        std::generate(_buffer.begin(), _buffer.end(), [&, n = 0]() mutable {
            float val = 0.0;
            for (uint8_t harmonic = 1; harmonic <= HIGHER_HARMONIC; harmonic++) {
                val += sinf(2.0 * M_PI * static_cast<float>(harmonic) *
                            static_cast<float>(n) / static_cast<float>(_buffer.size())) /
                       static_cast<float>(harmonic);
            }
            n++;
            return val;
        });
    }


}
