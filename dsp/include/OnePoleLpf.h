#pragma once

#include <cmath>

namespace dsp {

// source : https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/
// https://ccrma.stanford.edu/~jos/fp/One_Pole.html
// Make a High pass implementation?
class OnePoleLpf {
public:
  inline void setCuttOffFreq(double cuttOffFreq_hz) {
    b1 = exp(-2.0F * M_PI * cuttOffFreq_hz);
    a0 = 1.0F - b1;
  }

  inline void setFeedbackGain(float gain) {
    b1 = gain;
    a0 = 1.0F - b1;
  }

  inline float process(float in) { return z1 = in * a0 + z1 * b1; }

private:
  float a0 = 0.0F;
  float b1 = 0.0F;
  float z1 = 0.0F;
};

} // namespace dsp
