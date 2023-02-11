#include "WavetableOsc.h"

#include <iostream>

#include <AudioBufferTools.h>

#include <gmock/gmock.h>

using namespace synthtools;
using namespace testing;

struct ParamType {
  uint32_t tableSize = 10U;
  uint32_t framesPerBuffer = 2U;
  float sampleRate = 44100.0f;
  float freq = sampleRate / static_cast<float>(tableSize);
  float amplitude = .70f;
  utilities::AudioBufferTools::OscillatorType oscType =
      utilities::AudioBufferTools::OscillatorType::SINE;
};

class WavetableOscTest : public TestWithParam<ParamType> {
public:
  WavetableOscTest()
      : _waveTableOsc(utilities::AudioBufferTools::OscillatorType::SINE, 10U,
                      441000.f) {}

  void
  setUpExpectedBuffer(const utilities::AudioBufferTools::OscillatorType oscType,
                      const uint32_t oscTableSize, const float amplitude) {
    _expectedBuffer.resize(oscTableSize * _nChannels);
    std::vector<float> tempBuffer(oscTableSize);
    utilities::AudioBufferTools::makeOscTable(tempBuffer, oscType);
    std::for_each(tempBuffer.begin(), tempBuffer.end(),
                  [&amplitude](float &v) { v *= amplitude; });
    auto datPtr = _expectedBuffer.data();
    utilities::AudioBufferTools::monoToStereo(tempBuffer.data(),
                                              _expectedBuffer.data(),
                                              tempBuffer.size(), _nChannels);
  }

protected:
  void setUpOsc(const ParamType &testSettings) {
    _waveTableOsc.setUp(testSettings.oscType, testSettings.tableSize,
                        testSettings.sampleRate, true);
    _waveTableOsc.setFrequency(testSettings.freq);
    _waveTableOsc.setAmplitude(testSettings.amplitude);
  }

  void processBuffer(std::vector<float> &bufferToFill, const uint32_t nSamples,
                     const uint32_t framesPerBuffer) {
    for (int buffer_i = 0; buffer_i < nSamples / framesPerBuffer; ++buffer_i) {
      size_t offset = buffer_i * framesPerBuffer * _nChannels;
      utilities::AudioBuffer<float> audioBuffer(bufferToFill.data() + offset,
                                                _nChannels, framesPerBuffer);
      _waveTableOsc.process(audioBuffer);
    }
  }

  WavetableOsc _waveTableOsc;
  std::vector<float> _expectedBuffer;
  constexpr static uint32_t _nChannels = 2U;
};

TEST_P(WavetableOscTest, test_basic_osc) {
  // GIVEN
  ParamType testSettings = GetParam();
  setUpExpectedBuffer(testSettings.oscType, testSettings.tableSize,
                      testSettings.amplitude);
  setUpOsc(testSettings);
  std::vector<float> bufferToFill;
  bufferToFill.resize(testSettings.tableSize * _nChannels);
  // WHEN
  processBuffer(bufferToFill, testSettings.tableSize,
                testSettings.framesPerBuffer);
  // THEN
  ASSERT_EQ(bufferToFill, _expectedBuffer);
}

INSTANTIATE_TEST_SUITE_P(
    OscTypesTest, WavetableOscTest,
    Values(
        ParamType{.oscType = utilities::AudioBufferTools::OscillatorType::SINE},
        ParamType{
            .oscType = utilities::AudioBufferTools::OscillatorType::TRIANGLE},
        ParamType{
            .oscType = utilities::AudioBufferTools::OscillatorType::SQUARE},
        ParamType{
            .oscType = utilities::AudioBufferTools::OscillatorType::SAWTOOTH}),
    [](const TestParamInfo<ParamType> &info) {
      switch (info.param.oscType) {
      case utilities::AudioBufferTools::OscillatorType::SINE:
        return "sineOsc";
        break;
      case utilities::AudioBufferTools::OscillatorType::TRIANGLE:
        return "triangleOsc";
        break;
      case utilities::AudioBufferTools::OscillatorType::SQUARE:
        return "squareOsc";
        break;
      case utilities::AudioBufferTools::OscillatorType::SAWTOOTH:
        return "sawtoothOsc";
        break;
      default:
        return "none";
      }
    });

TEST_F(WavetableOscTest, test_free_osc) {
  const uint32_t tableSize = 100U;
  const uint32_t framesPerBuffer = 25U;
  const float sampleRate = 44'100.0f;
  float freq = sampleRate / static_cast<float>(tableSize);
  float amplitude = .70f;

  auto freeFuncOscBuilder = [](std::vector<float> &buffer) -> void {
    for (int i = 0; i < buffer.size(); ++i) {
      buffer[i] = static_cast<float>(i) * 100.0f;
    }
  };
  // Set expected buffer
  _expectedBuffer.resize(tableSize * _nChannels);
  std::vector<float> tempBuffer(tableSize);
  freeFuncOscBuilder(tempBuffer);
  std::for_each(tempBuffer.begin(), tempBuffer.end(),
                [&amplitude](float &v) { v *= amplitude; });
  utilities::AudioBufferTools::monoToStereo(
      tempBuffer.data(), _expectedBuffer.data(), tempBuffer.size(), _nChannels);

  // Set up oscillator
  _waveTableOsc.setUp(tableSize, sampleRate, freeFuncOscBuilder, true);
  _waveTableOsc.setFrequency(freq);
  _waveTableOsc.setAmplitude(amplitude);

  // WHEN
  std::vector<float> bufferToFill;
  bufferToFill.resize(tableSize * _nChannels);
  processBuffer(bufferToFill, tableSize, framesPerBuffer);

  // THEN
  ASSERT_EQ(bufferToFill, _expectedBuffer);
}
