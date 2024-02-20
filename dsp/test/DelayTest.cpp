#include "Delay.hpp"
#include <array>
#include <vector>

#include <gmock/gmock.h>

TEST(DelayTest, basicOps) {
  float sample_rate = 10000;
  float delay_ms = 100;
  dsp::Delay delay(delay_ms, sample_rate);
}