
#include "BiquadFilter.h"

#include <math.h>

namespace dsp
{

BiquadFilter::BiquadFilter(){

}

BiquadFilter::BiquadFilter(const FilterSettings& filterSettings)
{
  setUp(filterSettings);
  _z1 = _z2 = 0;
}

void BiquadFilter::setUp(const FilterSettings &filterSettings) {
  _filterSettings = filterSettings;
  update();
}

void BiquadFilter::setQfactor(double qFactor) {
  _filterSettings.qFactor = qFactor;
  update();
}

void BiquadFilter::setType(Type filterType) {
  _filterSettings.filterType = filterType;
  update();
}

void BiquadFilter::setCentralFreq(double cutoffFreq) {
  _filterSettings.cutoffFreq = cutoffFreq; 
  update();
}

void BiquadFilter::clear() { _z1 = _z2 = 0.0; }

void BiquadFilter::processBlock(float *inBuffer, float *outputBuffer,uint32_t numSamples){
  for(size_t sample = 0; sample  < numSamples; sample++){
    outputBuffer[sample] = process(inBuffer[sample]);
  }
}

void BiquadFilter::update() {
  auto K = 1 / std::tan(M_PI * 2.0 *  _filterSettings.cutoffFreq / _filterSettings.samplingFreq);
  auto K_2 = K*K;
  auto qInv = 1.0 / _filterSettings.qFactor;
  auto norm = 1.0 / (1.0 + K * qInv + K_2);
  switch (_filterSettings.filterType) {
  case Type::LOWPASS:
    _biquadCoeff.b0 = norm;
    _biquadCoeff.b1 = 2.0 * norm;
    _biquadCoeff.b2 = norm;
    _biquadCoeff.a1 = 1.0;
    _biquadCoeff.a2 = (1.0 - K * qInv + K_2) * norm;
    break;
  case Type::HIGHPASS:
    _biquadCoeff.b0 = norm;
    _biquadCoeff.b1 = -2.0 * _biquadCoeff.b0;
    _biquadCoeff.b2 = _biquadCoeff.b0;
    _biquadCoeff.a1 = 1.0;
    _biquadCoeff.a2 = (1.0 - K * qInv + K_2) * norm;

    break;
    break;
  case Type::BANDPASS:
    /* code */
    break;
  default:
    break;
  }
}



} // namespace AudioUtilities
