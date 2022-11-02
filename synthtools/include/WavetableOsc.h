#ifndef EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
#define EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H

#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>
#include <AudioBufferTools.h>
#include "IAudioRender.h"

namespace synthtools {

    using BuilderFunc = std::function<void(std::vector<float> &)>;

    float interpolation(const std::vector<float> &buffer, float indexPtr);

    class WavetableOsc : public IAudioRender {
    public:

        WavetableOsc() {}

        WavetableOsc(const utilities::AudioBufferTools::OscillatorType oscillatorType, const uint32_t tableSize,
                     const float sampleRate, const bool useInterpolation = true);

        void setUp(const utilities::AudioBufferTools::OscillatorType oscillatorType, const uint32_t tableSize,
                   const float sampleRate, const bool useInterpolation = true);

        void setUp(const uint32_t tableSize, const float sampleRate, BuilderFunc builder,
                   const bool useInterpolation = true);

        // Sets oscillator frequency
        void setFrequency(const float f);

        // Gets oscillator frequency
        float getFrequency() const;

        void setAmplitude(const float amplitude);

        // Gets next sample and update the phase
        float process();

        void renderAudio(float *audioData, uint32_t numFrames) override;

    private:
        void updateIncrement();
        utilities::AudioBufferTools::OscillatorType _oscillatorType = utilities::AudioBufferTools::OscillatorType::SAWTOOTH;
        std::vector<float> _buffer;
        float _readPointer = 0.0f;
        float _inverseSampleRate;
        float _freq;
        float _amplitude = 0.1;
        std::atomic<float> _tableIncrement = 0.0f;
        bool _useInterpolation = false;
    };


}
#endif //EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
