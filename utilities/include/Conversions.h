//
// Created by cesar on 11/14/22.
//

#ifndef EUTERPESND_CONVERSIONS_H
#define EUTERPESND_CONVERSIONS_H


#include <cstdint>
#include <cmath>

namespace utilities{

    class Conversions{
    public:
        __attribute_const__ static double pitchToFrequency(double pitch){
            double exponent = (pitch - static_cast<double>(MIDLE_C_PITH)) * static_cast<double>(1.0f / SEMITONES_PER_OCTAVE);
            return MIDLE_C_FREQUENCY * std::pow(2.0, exponent);
        }
    private:
        static constexpr uint32_t SEMITONES_PER_OCTAVE = 12U;
        static constexpr uint32_t MIDLE_C_PITH = 60U;
        static constexpr float MIDLE_C_FREQUENCY = 261.625549F;
    };


}

#endif //EUTERPESND_CONVERSIONS_H
