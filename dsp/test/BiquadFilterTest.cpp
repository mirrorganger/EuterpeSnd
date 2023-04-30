#include "BiquadFilter.h"
#include "AudioBufferTools.h"
#include "Conversions.h"
#include <array>
#include <vector>

#include <gmock/gmock.h>

using namespace dsp;
using namespace testing;

static const double _qFactor = 1.0 / sqrt(2.0);

struct FilterTestParams {
    const double cutoffFreq;
    const BiquadFilter::Type type;
    const std::vector<std::pair<double, double>> expectedGains;
};

const std::vector<std::pair<double, double>> expectedGGainsLP = {
        {1.0, -3.0},
        {2.0, -12.0},
        {4.0, -24.0}};
const std::vector<std::pair<double, double>> expectedGGainsHP = {
        {1.0, -3.0},
        {.5,  -12.0},
        {.25, -24.0}};

// TODO : Check dB numbers
const std::vector<std::pair<double, double>> expectedGGainsBP = {
        {1.0, 0.0},
        {0.25,  -9.0},
        {4.0, -9.0}};

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
    BiquadFilterTest() {}

protected:
    //double _qFactor = 1.0 / sqrt(2.0);
    double _samplingFreq = 48000.0;
};

TEST_P(BiquadFilterTest, filterTest) {
    auto param = GetParam();
    BiquadFilter::FilterSettings settings{};
    settings.cutoffFreq = param.cutoffFreq;
    settings.nChannels = 2U;
    settings.qFactor = _qFactor;
    settings.samplingFreq = _samplingFreq;
    settings.filterType = param.type;
    for (auto freqAndMag: param.expectedGains) {
        double freq = param.cutoffFreq * freqAndMag.first;
        testFilter(settings, freq, freqAndMag.second, 0.7);
    }
}

INSTANTIATE_TEST_SUITE_P(
        allFiltersTests, BiquadFilterTest,
        Values(
                // clang-format off
                FilterTestParams{100.0, BiquadFilter::Type::LOWPASS, expectedGGainsLP},
                FilterTestParams{200.0f, BiquadFilter::Type::LOWPASS, expectedGGainsLP},
                FilterTestParams{500.0, BiquadFilter::Type::LOWPASS, expectedGGainsLP},
                FilterTestParams{1000.0f, BiquadFilter::Type::LOWPASS, expectedGGainsLP},
                FilterTestParams{100.0f, BiquadFilter::Type::HIGHPASS, expectedGGainsHP},
                FilterTestParams{200.0f, BiquadFilter::Type::HIGHPASS, expectedGGainsHP},
                FilterTestParams{500.0, BiquadFilter::Type::HIGHPASS, expectedGGainsHP},
                FilterTestParams{1000.0, BiquadFilter::Type::HIGHPASS, expectedGGainsHP},
                FilterTestParams{100.0, BiquadFilter::Type::BANDPASS, expectedGGainsBP},
                FilterTestParams{200.0, BiquadFilter::Type::BANDPASS, expectedGGainsBP},
                FilterTestParams{500.0, BiquadFilter::Type::BANDPASS, expectedGGainsBP},
                FilterTestParams{1000.0, BiquadFilter::Type::BANDPASS, expectedGGainsBP}
                ));
                // clang-format on
