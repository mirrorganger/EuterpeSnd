#ifndef EUTERPESND_DSP_BIQUAD_FILTER
#define EUTERPESND_DSP_BIQUAD_FILTER

#include <cmath>
#include <cstdint>
namespace dsp

{

class BiquadFilter {

public:
    enum class Type {
        LOWPASS,
        HIGHPASS,
        BANDPASS
    };

    struct FilterSettings
    {
       double samplingFreq;
       double qFactor;
       double cutoffFreq;
       Type filterType;
    };

    struct BiquadCoeff{
        double b0,b1,b2,a1,a2;
    };    

    BiquadFilter();
    BiquadFilter(const FilterSettings& filterSettings);

    void setUp(const FilterSettings& filterSettings);

    void setQfactor(double qFactor);
    void setType(Type filterType);
    void setCentralFreq(double centralFreq);
    void clear();
    float process(float inSample);
    void processBlock(float *inBuffer,float * outBuffer,uint32_t numSamples);

private:
    void update();
    FilterSettings _filterSettings{0};
    double _z1, _z2;
    BiquadCoeff _biquadCoeff;
};


inline float BiquadFilter::process(float inSample){
    double out = _biquadCoeff.b0 * inSample + _z1;
    _z1 = (inSample * _biquadCoeff.b1) - (_biquadCoeff.a1 * out) + _z2;
    _z2 = (inSample * _biquadCoeff.b2) - (_biquadCoeff.a2 * out);  
    return out;
}



}


#endif