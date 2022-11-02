#include "WavetableOsc.h"

#include <iostream>

#include <AudioBufferTools.h>

#include <gmock/gmock.h>



using namespace synthtools;
using namespace testing;

class WavetableOscTest : public Test {
public:
    WavetableOscTest() = default;

    void setUpExpectedBuffer(const utilities::AudioBufferTools::OscillatorType oscType,const uint32_t oscTableSize,const float amplitude){
        _buffer.resize(oscTableSize);
        utilities::AudioBufferTools::makeOscTable(_buffer,oscType);
        std::for_each(_buffer.begin(), _buffer.end(), [&amplitude](float &v) { v *= amplitude; });
    }

protected:
    WavetableOsc _waveTableOsc;
    std::vector<float> _buffer;
};

TEST_F(WavetableOscTest, test_sine_osc) {
    // GIVEN
    const uint32_t tableSize = 10U;
    const float sampleRate = 44100.0f;
    const float freq = sampleRate / static_cast<float>(tableSize);
    const float amplitude = .70f;
    const auto oscType = utilities::AudioBufferTools::OscillatorType::SINE;
    setUpExpectedBuffer(oscType, tableSize, amplitude);
    _waveTableOsc.setUp(oscType, tableSize, sampleRate, true);
    _waveTableOsc.setFrequency(freq);
    _waveTableOsc.setAmplitude(amplitude);
    // WHEN
    std::array<float, tableSize> bufferToFill{};
    _waveTableOsc.renderAudio(bufferToFill.data(), bufferToFill.size());

    // THEN
    ASSERT_THAT(bufferToFill, ElementsAreArray(_buffer));
}

TEST_F(WavetableOscTest, test_triangle_osc) {
    // GIVEN
    const uint32_t tableSize = 100U;
    const float sampleRate = 44100.0f;
    const float freq = sampleRate / static_cast<float>(tableSize);
    const float amplitude = .50f;
    const auto oscType = utilities::AudioBufferTools::OscillatorType::TRIANGLE;
    setUpExpectedBuffer(oscType, tableSize, amplitude);
    _waveTableOsc.setUp(oscType, tableSize, sampleRate, true);
    _waveTableOsc.setFrequency(freq);
    _waveTableOsc.setAmplitude(amplitude);
    // WHEN
    std::array<float, tableSize> bufferToFill{};
    _waveTableOsc.renderAudio(bufferToFill.data(), bufferToFill.size());

    // THEN
    ASSERT_THAT(bufferToFill, ElementsAreArray(_buffer));
}

TEST_F(WavetableOscTest, test_square_osc) {
    // GIVEN
    const uint32_t tableSize = 100U;
    const float sampleRate = 44100.0f;
    const float freq = sampleRate / static_cast<float>(tableSize);
    const float amplitude = .50f;
    const auto oscType = utilities::AudioBufferTools::OscillatorType::SQUARE;
    setUpExpectedBuffer(oscType, tableSize, amplitude);
    _waveTableOsc.setUp(oscType, tableSize, sampleRate, true);
    _waveTableOsc.setFrequency(freq);
    _waveTableOsc.setAmplitude(amplitude);
    // WHEN
    std::array<float, tableSize> bufferToFill{};
    _waveTableOsc.renderAudio(bufferToFill.data(), bufferToFill.size());

    // THEN
    ASSERT_THAT(bufferToFill, ElementsAreArray(_buffer));
}

TEST_F(WavetableOscTest, test_sawtooth_osc) {
    // GIVEN
    const uint32_t tableSize = 100U;
    const float sampleRate = 44100.0f;
    const float freq = sampleRate / static_cast<float>(tableSize);
    const float amplitude = .50f;
    const auto oscType = utilities::AudioBufferTools::OscillatorType::SAWTOOTH;
    setUpExpectedBuffer(oscType, tableSize, amplitude);
    _waveTableOsc.setUp(oscType, tableSize, sampleRate, true);
    _waveTableOsc.setFrequency(freq);
    _waveTableOsc.setAmplitude(amplitude);
    // WHEN
    std::array<float, tableSize> bufferToFill{};
    _waveTableOsc.renderAudio(bufferToFill.data(), bufferToFill.size());

    // THEN
    ASSERT_THAT(bufferToFill, ElementsAreArray(_buffer));
}






