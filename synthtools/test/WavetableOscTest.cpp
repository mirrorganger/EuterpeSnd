#include "WavetableOsc.h"

#include <gmock/gmock.h>

#include <iostream>

using namespace synthtools;
using namespace testing;

class WavetableOscTest : public Test {
public:
    WavetableOscTest() {
    }

    void makeSineOscillator() {
        std::generate(_buffer.begin(), _buffer.end(), [&, n = 0]() mutable {
            return sinf(2.0 * M_PI * static_cast<float>(n++) /
                        static_cast<float>(_buffer.size()));
        });
    }

    void makeTriangleOscillator() {
        std::generate(_buffer.begin(), _buffer.begin() + _buffer.size() / 2,
                      [&, n = 0]() mutable {
                          return -1.0 + 4.0 * static_cast<float>(n++) /
                                        static_cast<float>(_buffer.size());
                      });
        std::generate(_buffer.begin() + _buffer.size() / 2, _buffer.end(),
                      [&, n = _buffer.size() / 2]() mutable {
                          return 1.0 -
                                 4.0 * static_cast<float>(n++ - _buffer.size() / 2) /
                                 static_cast<float>(_buffer.size());
                      });
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

    _buffer.resize(tableSize);
    makeSineOscillator();
    std::for_each(_buffer.begin(),_buffer.end(),[&amplitude](float& v){v*=amplitude;});
    _waveTableOsc.setUp(WavetableOsc::OscillatorType::SINE, tableSize, sampleRate, true);
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
    _buffer.resize(tableSize);
    makeTriangleOscillator();
    std::for_each(_buffer.begin(),_buffer.end(),[&amplitude](float& v){v*=amplitude;});
    _waveTableOsc.setUp(WavetableOsc::OscillatorType::TRIANGLE, tableSize, sampleRate, true);
    _waveTableOsc.setFrequency(freq);
    _waveTableOsc.setAmplitude(amplitude);
    // WHEN
    std::array<float, tableSize> bufferToFill{};
    _waveTableOsc.renderAudio(bufferToFill.data(), bufferToFill.size());

    // THEN
    ASSERT_THAT(bufferToFill, ElementsAreArray(_buffer));
}





