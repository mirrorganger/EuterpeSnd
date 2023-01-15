#ifndef EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
#define EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H

#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>
#include <AudioBufferTools.h>
#include "AudioBuffer.h"

namespace synthtools {

    using BuilderFunc = std::function<void(std::vector<float> &)>;

    float interpolation(const std::vector<float> &buffer, float indexPtr);

    class WavetableOsc : public utilities::AudioProcessor<float> {
    public:

        WavetableOsc(const utilities::AudioBufferTools::OscillatorType oscillatorType, const uint32_t tableSize,
                     const float sampleRate, const bool useInterpolation = true);

        void setUp(const utilities::AudioBufferTools::OscillatorType oscillatorType, const uint32_t tableSize,
                   const float sampleRate, const bool useInterpolation = true);

        void setUp(const uint32_t tableSize, const float sampleRate, BuilderFunc builder,
                   const bool useInterpolation = true);

        void setFrequency(const float f);

        void setAmplitude(const float amplitude);

        // Gets next sample and update the phase
        float process();

        void process(utilities::AudioBuffer<float>& buffer);

    private:
        void updateIncrement();
        std::vector<float> _buffer;
        float _readPointer = 0.0f;
        float _inverseSampleRate;
        float _freq;
        float _amplitude = 1.0;
        std::atomic<float> _tableIncrement = 0.0f;
        bool _useInterpolation = false;
    };


}
#endif //EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
