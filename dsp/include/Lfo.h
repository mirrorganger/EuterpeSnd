#pragma once

#include <cmath>
namespace dsp
{
    class Lfo {
    public:
        using NumericType = float;
        enum class WaveFormT { TRIANGLE, SINE, SAW };
        Lfo(NumericType samplingFreq_hz, NumericType oscFreq_hz, WaveFormT type = WaveFormT::TRIANGLE) ;
        void reset(NumericType samplingFreq_hz, NumericType oscFreq_hz);
        NumericType get();
    private:
        void advanceCounter();
        const NumericType TWO_PI = static_cast<NumericType>(2.0) * static_cast<NumericType>(M_PI);
        NumericType _freq_hz;
        WaveFormT _type;
        NumericType _phaseIncrement;
        NumericType _moduloCounter = NumericType{};
    };
} // namespace dsp