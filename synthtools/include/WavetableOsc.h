#ifndef EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
#define EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H

#include <cstdint>
#include <vector>
#include <functional>

namespace synthtools {

    using BuilderFunc = std::function<void(std::vector < float > &)>;

    float interpolation(const std::vector<float> &buffer, float indexPtr);

    class WavetableOsc {
    public:

        enum class OscillatorType : uint8_t {
            SINE, SAWTOOTH, TRIANGLE, SQUARE, FREE
        };

        WavetableOsc() {}

        WavetableOsc(const OscillatorType oscillatorType, const uint32_t tableSize,
                     const float sampleRate, const bool useInterpolation = true);

        void setUp(const OscillatorType oscillatorType, const uint32_t tableSize,
                   const float sampleRate, const bool useInterpolation = true);

        void setUp(const uint32_t tableSize, const float sampleRate, BuilderFunc builder,
                   const bool useInterpolation = true);

        // Sets oscillator frequency
        void setFrequency(const float f);

        // Gets oscillator frequency
        float getFrequency() const;

        // Gets next sample and update the phase
        float process();

        void process(float *bufferToFill, uint32_t numFrames);

    private:
        void updateIncrement();

        void makeTriangleOscillator();

        void makeSineOscillator();

        void makeSquareOscillator();

        void makeSawToothOscillator();

        OscillatorType _oscillatorType = OscillatorType::SAWTOOTH;
        std::vector<float> _buffer;
        float _readPointer = 0.0f;
        float _inverseSampleRate;
        float _freq;
        float _tableIncrement = 0.0f;
        bool _useInterpolation = false;
    };


}
#endif //EUTERPESND_SYNTHTOOLS_WAVETABLEOSC_H
