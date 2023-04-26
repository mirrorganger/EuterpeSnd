#ifndef EUTERPESND_DSP_BIQUAD_FILTER
#define EUTERPESND_DSP_BIQUAD_FILTER

#include "AudioBuffer.h"

#include <cmath>
#include <cstdint>
#include <vector>

namespace dsp {

using NumericDataType = float;

class BiquadFilter : utilities::AudioProcessor<NumericDataType> {
public:
  enum class Type : uint8_t { LOWPASS, HIGHPASS, BANDPASS };

  struct FilterSettings {
    double samplingFreq;
    double qFactor;
    double cutoffFreq;
    Type filterType;
    uint32_t nChannels;
  };

  struct BiquadCoeff {
    double b0, b1, b2, a1, a2;
  };

  BiquadFilter();
  BiquadFilter(const FilterSettings &filterSettings);

  void setUp(const FilterSettings &filterSettings);

  // TODO :All these setters call to update, and change the
  // biquad coeficients from another thread. Create a spin lock mutex??
  void setQfactor(double qFactor);
  void setType(Type filterType);
  void setCentralFreq(double centralFreq);
  void clear();
  void process(utilities::AudioBuffer<NumericDataType> &buffer);

private:
  void reset();
  void update();
  FilterSettings _filterSettings{0};
  std::vector<double> _z1{}, _z2{};
  BiquadCoeff _biquadCoeff;
};

} // namespace dsp

#endif