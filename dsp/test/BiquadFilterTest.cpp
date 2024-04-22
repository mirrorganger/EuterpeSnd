#include "BiquadFilter.h"
#include "AudioBufferTools.h"
#include "Conversions.h"
#include <array>
#include <vector>

#include <gmock/gmock.h>

using namespace dsp;
using namespace testing;

static const double Q_FACTOR = 1.0 / sqrt(2.0);
static const double Q_FACTOR_BANDPSASS = sqrt(2.0);

struct FilterTestParams {
  const double cutoffFreq;
  const BiquadFilter::Type type;
  const double qFactor;
  const double gain_db;
  const std::vector<std::pair<double, double>> expectedGains;
};

const std::vector<std::pair<double, double>> expectedGGainsLP = {
    {1.0, -3.0}, {2.0, -12.0}, {4.0, -24.0}};
const std::vector<std::pair<double, double>> expectedGGainsHP = {
    {1.0, -3.0}, {.5, -12.0}, {.25, -24.0}};

template<double BoostGain = 12.0>
const std::vector<std::pair<double,double>> expectedGainsLS = {
    {.3, BoostGain}, {5.0 , 0.0}};

template<double BoostGain = 12.0>
const std::vector<std::pair<double,double>> expectedGainsHS = {
    {.1, 0.0},{5.0, BoostGain}};


std::vector<FilterTestParams> getBandPassTestParams() {
  std::vector<FilterTestParams> params;
  std::vector<std::pair<double, double>> bandLimitsHz = {
      {1000.0, 2000.0}, {300.0, 600.0}, {800.0, 1600.0}};

  for (auto limit : bandLimitsHz) {
    auto f_center = sqrt(limit.second * limit.first);
    const std::vector<std::pair<double, double>> expectedGGainsBP = {
        {1.0, 0.0},
        {limit.first / f_center, -3.0},
        {limit.second / f_center, -3.0}};
    params.emplace_back(FilterTestParams{f_center, BiquadFilter::Type::BANDPASS,
                                         Q_FACTOR_BANDPSASS, 1.0,
                                         expectedGGainsBP});
  }
  return params;
};

void testFilter(const BiquadFilter::FilterSettings &settings, const double freq,
                const double expectedGain, const double allowedError) {
  double length_s = 1.0;
  auto numSamples = static_cast<size_t>(settings.samplingFreq * length_s);

  BiquadFilter filter(settings);

  auto buffer = utilities::AudioBufferTools::makeSineWave(
      freq, settings.samplingFreq, static_cast<uint32_t>(numSamples),
      settings.nChannels);
  utilities::AudioBuffer<float> audioBuffer(buffer.data(), settings.nChannels,
                                            numSamples);

  filter.process(audioBuffer);
  auto halfSamples = audioBuffer.getFramesPerBuffer() / 2U;
  for (size_t channel_i = 0U; channel_i < settings.nChannels; ++channel_i) {
    auto maxVal = utilities::getChannelMag(audioBuffer, halfSamples,
                                           halfSamples - 1, channel_i);
    auto magDb = utilities::fromGainToDecibels(maxVal);
    EXPECT_NEAR(magDb, expectedGain, allowedError);
  }
}

class BiquadFilterTest : public TestWithParam<FilterTestParams> {
public:
protected:
  double _samplingFreq = 48000.0;
};

TEST_P(BiquadFilterTest, filterTest) {
  auto param = GetParam();
  BiquadFilter::FilterSettings settings{};
  settings.cutoffFreq = param.cutoffFreq;
  settings.nChannels = 2U;
  settings.qFactor = param.qFactor;
  settings.samplingFreq = _samplingFreq;
  settings.filterType = param.type;
  settings.gain_db = param.gain_db;
  for (auto freqAndMag : param.expectedGains) {
    double freq = param.cutoffFreq * freqAndMag.first;
    testFilter(settings, freq, freqAndMag.second, 0.9);
  }
}

INSTANTIATE_TEST_SUITE_P(
    lowPassHighPassTests, BiquadFilterTest,
    Values(
        // clang-format off
                FilterTestParams{100.0, BiquadFilter::Type::LOWPASS,   Q_FACTOR, 1.0, expectedGGainsLP},
                FilterTestParams{200.0f, BiquadFilter::Type::LOWPASS,  Q_FACTOR, 1.0, expectedGGainsLP,},
                FilterTestParams{500.0, BiquadFilter::Type::LOWPASS,   Q_FACTOR, 1.0,expectedGGainsLP},
                FilterTestParams{1000.0f, BiquadFilter::Type::LOWPASS, Q_FACTOR, 1.0, expectedGGainsLP},
                FilterTestParams{100.0f, BiquadFilter::Type::HIGHPASS, Q_FACTOR, 1.0,expectedGGainsHP},
                FilterTestParams{200.0f, BiquadFilter::Type::HIGHPASS, Q_FACTOR, 1.0,expectedGGainsHP},
                FilterTestParams{500.0, BiquadFilter::Type::HIGHPASS,  Q_FACTOR ,1.0, expectedGGainsHP},
                FilterTestParams{1000.0, BiquadFilter::Type::HIGHPASS, Q_FACTOR ,1.0, expectedGGainsHP}, 
                FilterTestParams{800.0, BiquadFilter::Type::LOW_SHELV, Q_FACTOR ,12.0, expectedGainsLS<12.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::LOW_SHELV, Q_FACTOR ,2.0, expectedGainsLS<2.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::LOW_SHELV, Q_FACTOR ,-12.0, expectedGainsLS<-12.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::LOW_SHELV, Q_FACTOR ,-2.0, expectedGainsLS<-2.0>},
                FilterTestParams{800.0, BiquadFilter::Type::HIGH_SHELV, Q_FACTOR ,12.0, expectedGainsHS<12.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::HIGH_SHELV, Q_FACTOR ,2.0, expectedGainsHS<2.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::HIGH_SHELV, Q_FACTOR ,-12.0, expectedGainsHS<-12.0>},                 
                FilterTestParams{800.0, BiquadFilter::Type::HIGH_SHELV, Q_FACTOR ,-2.0, expectedGainsHS<-2.0>}
          //clang-format on
        ));

INSTANTIATE_TEST_SUITE_P(bandPassTests, BiquadFilterTest,
                         ValuesIn(getBandPassTestParams()));
