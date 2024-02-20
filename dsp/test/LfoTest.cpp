#include "Lfo.h"

#include <gmock/gmock.h>

using namespace testing;

template <typename DataType>
void expectBoundedBuffer(const std::vector<DataType> &buffer,
                         DataType lowerBound, DataType upperBound,
                         double_t MAX_ERROR = 1E-6) {
  auto max = *std::max_element(buffer.begin(), buffer.end());
  auto min = *std::min_element(buffer.begin(), buffer.end());
  EXPECT_NEAR(max, upperBound, MAX_ERROR);
  EXPECT_NEAR(min, lowerBound, MAX_ERROR);
}

template <typename DataType>
void expectDecreasingBuffer(const std::vector<DataType> &buffer,
                            double_t MAX_ERROR = 1E-6) {
  auto prevSample = std::numeric_limits<DataType>::max();
  for (const auto &sample : buffer) {
    if (sample > prevSample) {
      EXPECT_NEAR(sample, prevSample, MAX_ERROR);
    }
    prevSample = sample;
  }
}

template <typename DataType>
void expectIncreasingBuffer(const std::vector<DataType> &buffer,
                            double_t MAX_ERROR = 1E-6) {
  auto prevSample = std::numeric_limits<DataType>::lowest();
  for (const auto &sample : buffer) {
    if (sample < prevSample) {
      EXPECT_NEAR(sample, prevSample, MAX_ERROR);
    }
    prevSample = sample;
  }
}

TEST(LfoTest, triangleWaweForm) {

  // GIVEN
  constexpr float sampleRate_hz = 1000.0F;
  constexpr float oscFreq_hz = 10.0F;
  constexpr double_t MAX_ERROR = 1E-4;
  constexpr auto stepsPerPeriod =
      static_cast<size_t>(sampleRate_hz / oscFreq_hz);
  dsp::Lfo lfo(sampleRate_hz, oscFreq_hz, dsp::Lfo::WaveFormT::TRIANGLE);

  // WHEN

  auto modulation = std::vector<dsp::Lfo::NumericType>(stepsPerPeriod + 1U);

  std::for_each(modulation.begin(), modulation.end(),
                [&lfo](auto &v) { v = lfo(); });

  // THEN
  EXPECT_NEAR(modulation[0], 1.0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod / 2], -1.0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod], 1.0F, MAX_ERROR);
  expectDecreasingBuffer(std::vector(modulation.begin(),
                                     modulation.begin() + (stepsPerPeriod / 2)),
                         MAX_ERROR);
  expectIncreasingBuffer(std::vector(modulation.begin() + (stepsPerPeriod / 2),
                                     modulation.end() - 1),
                         MAX_ERROR);
  expectBoundedBuffer(modulation, -1.0F, 1.0F, MAX_ERROR);
}

TEST(LfoTest, sawWaweForm) {

  // GIVEN
  constexpr float sampleRate_hz = 1000.0F;
  constexpr float oscFreq_hz = 10.0F;
  constexpr double_t MAX_ERROR = 1E-4;
  constexpr auto stepsPerPeriod =
      static_cast<size_t>(sampleRate_hz / oscFreq_hz);
  dsp::Lfo lfo(sampleRate_hz, oscFreq_hz, dsp::Lfo::WaveFormT::SAW);

  // WHEN
  auto modulation = std::vector<dsp::Lfo::NumericType>(stepsPerPeriod + 1U);
  std::for_each(modulation.begin(), modulation.end(),
                [&lfo](auto &v) { v = lfo(); });

  // THEN
  EXPECT_NEAR(modulation[0], -1.0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod / 2], .0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod], 1.0F, MAX_ERROR);
  expectIncreasingBuffer(modulation, MAX_ERROR);
  expectBoundedBuffer(modulation, -1.0F, 1.0F, MAX_ERROR);
}

TEST(LfoTest, sineWaweForm) {

  // GIVEN
  constexpr float sampleRate_hz = 1000.0F;
  constexpr float oscFreq_hz = 10.0F;
  constexpr double_t MAX_ERROR = 1E-4;
  constexpr auto stepsPerPeriod =
      static_cast<size_t>(sampleRate_hz / oscFreq_hz);
  dsp::Lfo lfo(sampleRate_hz, oscFreq_hz, dsp::Lfo::WaveFormT::SINE);

  // WHEN
  auto modulation = std::vector<dsp::Lfo::NumericType>(stepsPerPeriod + 1U);
  std::for_each(modulation.begin(), modulation.end(),
                [&lfo](auto &v) { v = lfo(); });

  // THEN
  EXPECT_NEAR(modulation[0], .0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod / 4], 1.0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod / 2], .0F, MAX_ERROR);
  EXPECT_NEAR(modulation[3 * stepsPerPeriod / 4], -1.0F, MAX_ERROR);
  EXPECT_NEAR(modulation[stepsPerPeriod], .0F, MAX_ERROR);
  expectBoundedBuffer(modulation, -1.0F, 1.0F, MAX_ERROR);
}
