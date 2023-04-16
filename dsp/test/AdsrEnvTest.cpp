#include "AdsrEnv.h"

#include <gmock/gmock.h>

using namespace dsp;
using namespace testing;
using namespace std::chrono_literals;

/**
 *
 * TODO : A possible idea to make this test more "real" is to check if the processed
 * byffer is increasing. As JUCE does in its ADSRTest.
 *  - Get test buffer of the required length based on the envelope section time
 *  - Apply envelope
 *  - Check the buffer is increasing / decreasing.
 *  - It will require test helpers to check
 *      - A buffer is increasing.
 *      - A buffer is descreasing.
 *      - A buffer is sustained.
 *
 */

template<typename T>
static bool isBufferDecreasing(const utilities::AudioBuffer<T> &buffer) {
    for (uint32_t channel = 0; channel < buffer.getNumberOfChannels(); ++channel) {
        T prevSample = std::numeric_limits<T>::max();
        for (int sample = 0; sample < buffer.getFramesPerBuffer(); ++sample) {
            auto currentSample = buffer[sample * buffer.getNumberOfChannels() + channel];
            if (currentSample >= prevSample) {
                std::cout << currentSample << ", " << prevSample;
                return false;
            }
            prevSample = currentSample;
        }
    }
    return true;
}

template<typename T>
static bool isBufferIncreasing(const utilities::AudioBuffer<T> &buffer) {
    for (uint32_t channel = 0; channel < buffer.getNumberOfChannels(); ++channel) {
        T prevSample = std::numeric_limits<T>::min();
        for (int sample = 0; sample < buffer.getFramesPerBuffer(); ++sample) {
            auto currentSample = buffer[sample * buffer.getNumberOfChannels() + channel];
            if (currentSample <= prevSample) {
                std::cout << currentSample << ", " << prevSample;
                return false;
            }
            prevSample = currentSample;
        }
    }
    return true;
}
class ADSRTest : public Test {
public:
    using Duration_ms = std::chrono::duration<double, std::milli>;

    ADSRTest() {
        _parameters.attackTime = 100.0ms;
        _parameters.decayTime = 30.0ms;
        _parameters.releaseTime = 400.0ms;
        _parameters.sustainLevel = 0.6;
        _adsrEnv.configure(_parameters);
        _adsrEnv.setSampleRate(static_cast<double>(_samplingFreq_hz));
    }

    double advanceEnv(AdsrEnv &env, const std::chrono::duration<double> &duration) {
        auto numTicks = static_cast<uint64_t>(duration.count() * _samplingFreq_hz);
        double val = 0.0;
        for (uint64_t i = 0U; i < numTicks; ++i) {
            val = env.tick();
        }
        return val;
    }

protected:
    AdsrEnv _adsrEnv;
    double _samplingFreq_hz = 100.0;
    AdsrEnv::Parameters _parameters;
};

TEST_F(ADSRTest, initialNoActive) {
    EXPECT_TRUE(!_adsrEnv.isActive());
    for (int times = 0; times < 100; ++times) {
        EXPECT_FLOAT_EQ(_adsrEnv.tick(), 0.0f);
    }
    EXPECT_TRUE(!_adsrEnv.isActive());
}

TEST_F(ADSRTest, completeAnEnvelope) {
    // GIVEN
    double val = 0.0;
    double minimumLevel = 0.0001;
    //// TRIGGER
    // WHEN
    _adsrEnv.trigger();
    // THEN
    ASSERT_TRUE(_adsrEnv.isActive());
    //// ATTACK
    //WHEN
    val = advanceEnv(_adsrEnv, _parameters.attackTime);
    // THEN
    EXPECT_FLOAT_EQ(val, 1.0);
    //// SUSTAIN
    // WHEN
    val = advanceEnv(_adsrEnv, _parameters.decayTime);
    // THEN
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);
    //// SUSTAIN
    val = advanceEnv(_adsrEnv, 1000.0ms);
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);
    //// RELEASE
    // WHEN
    _adsrEnv.release();
    val = advanceEnv(_adsrEnv, _parameters.releaseTime);
    // THEN
    //// IDLE
    EXPECT_TRUE(!_adsrEnv.isActive());
}


TEST_F(ADSRTest, Attack) {
    // GIVEN
    auto ticksAttack = static_cast<uint64_t>(AdsrEnv::SecondsDur(_parameters.attackTime).count() * _samplingFreq_hz);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(ticksAttack * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, ticksAttack);
    // WHEN
    _adsrEnv.trigger();
    // THEN
    ASSERT_TRUE(_adsrEnv.isActive());
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferIncreasing(bufferToFill));
    EXPECT_FLOAT_EQ(buffer[buffer.size() - 1], 1.0);
}

TEST_F(ADSRTest, Decay) {
    // GIVEN
    auto tickDecay = static_cast<int64_t>(AdsrEnv::SecondsDur(_parameters.decayTime).count() * _samplingFreq_hz);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(tickDecay * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, tickDecay);

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime);
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferDecreasing(bufferToFill));
    EXPECT_FLOAT_EQ(buffer[buffer.size() - 1], _parameters.sustainLevel);

}

TEST_F(ADSRTest, Sustain) {
    // GIVEN
    auto numberOfticks = static_cast<uint64_t>(2.0 * _samplingFreq_hz);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(numberOfticks * nChannels);
    std::vector<float> expectedBuffer(numberOfticks * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    std::fill(expectedBuffer.begin(), expectedBuffer.end(), _parameters.sustainLevel);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, numberOfticks);

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime + _parameters.decayTime);
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    EXPECT_EQ(buffer, expectedBuffer);
}


TEST_F(ADSRTest, Release) {
    // GIVEN
    auto ticksRelease = static_cast<uint64_t>(AdsrEnv::SecondsDur(_parameters.releaseTime).count() * _samplingFreq_hz);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(ticksRelease * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, ticksRelease);
    double minimumLevel = 0.0001;

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime + _parameters.decayTime);
    auto val = advanceEnv(_adsrEnv, 100ms);
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);
    _adsrEnv.release();

    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferDecreasing(bufferToFill));
}




