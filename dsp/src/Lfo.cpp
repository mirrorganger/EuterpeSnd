#include "Lfo.h"

#include <cmath>

namespace dsp
{

Lfo::Lfo(Lfo::NumericType samplingFreq_hz, Lfo::NumericType oscFreq_hz, Lfo::WaveFormT type)
      : _freq_hz(oscFreq_hz), _type(type),
        _phaseIncrement(_freq_hz / samplingFreq_hz) {

}

void Lfo::reset(Lfo::NumericType samplingFreq_hz, Lfo::NumericType oscFreq_hz){
    _freq_hz = oscFreq_hz;
    _phaseIncrement = _freq_hz / samplingFreq_hz;
    _moduloCounter = 0.0;
}
  
Lfo::NumericType Lfo::getNextSample(){
    NumericType out = {};
    switch (_type)
    {
    case Lfo::WaveFormT::SAW:
        out = static_cast<NumericType>(2.0 * _moduloCounter - 1.0); // unipolar to bipolar
        break;
    case Lfo::WaveFormT::TRIANGLE:
        out = static_cast<NumericType>(2.0 * _moduloCounter - 1.0); // unipolar to bipolar
        out = static_cast<NumericType>(2.0 * fabs(out) - 1.0); // bipolar to triangle    
        break;
    case Lfo::WaveFormT::SINE:
        out = sinf(_moduloCounter * TWO_PI);
        break;
    default:
        break;
    }
    advanceCounter();
    return out;
}

void Lfo::advanceCounter()
{
    _moduloCounter += _phaseIncrement;

    if(_phaseIncrement > 0 && _moduloCounter >= 1.0){
        _moduloCounter -=NumericType{1.0};
    }    

    if(_phaseIncrement < 0 && _moduloCounter <= 0.0){
        _moduloCounter +=NumericType{1.0};
    }    
}   


}


