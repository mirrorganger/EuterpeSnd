#include "AdsrEnv.h"

#include <gmock/gmock.h>

using namespace dsp;
using namespace testing;
using namespace std::chrono_literals;

template<typename T>
static bool isBufferDecreasing(const utilities::AudioBuffer<T> &buffer) {
    for (uint32_t channel = 0; channel < buffer.getNumberOfChannels(); ++channel) {
        T prevSample = std::numeric_limits<T>::max();
        for (int sample = 0; sample < buffer.getFramesPerBuffer(); ++sample) {
            auto currentSample = buffer[sample * buffer.getNumberOfChannels() + channel];
            if (currentSample >= prevSample) {
                EXPECT_NEAR(currentSample, prevSample, 0.000001);
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
                EXPECT_NEAR(currentSample, prevSample, 0.000001);
            }
            prevSample = currentSample;
        }
    }
    return true;
}

template<typename NumericType>
NumericType getExpRateMultiplier(NumericType startLevel, NumericType endLevel, NumericType sampleRate,
                                 AdsrEnv::SecondsDur intervalTime) {
    return 1.0 + (log(endLevel) - log(startLevel)) / (intervalTime.count() * sampleRate);
}

template<typename NumericType>
uint64_t
getLengthFromMultiplier(NumericType startLevel, NumericType endLevel, NumericType sampleRate, NumericType multiplier) {
    return static_cast<uint64_t>(log(endLevel) - log(startLevel)) / (multiplier - 1.0 * sampleRate);
}

template<typename NumericType>
uint64_t getSectionSamplesLeft(NumericType originalStartLevel, NumericType endLevel, NumericType samplingFreq_hz,
                               AdsrEnv::SecondsDur originalTime, NumericType newStart) {
    auto attackMultiplier = getExpRateMultiplier(originalStartLevel, endLevel, samplingFreq_hz, originalTime);
    return getLengthFromMultiplier(newStart, endLevel, samplingFreq_hz, attackMultiplier);
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
    double _samplingFreq_hz = 48'000.0;
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
    EXPECT_NEAR(val, 1.0, MINIMUM_LEVEL);
    //// SUSTAIN
    // WHEN
    val = advanceEnv(_adsrEnv, _parameters.decayTime);
    // THEN
    EXPECT_NEAR(val, _parameters.sustainLevel, MINIMUM_LEVEL);
    //// SUSTAIN
    val = advanceEnv(_adsrEnv, 1000.0ms);
    EXPECT_NEAR(val, _parameters.sustainLevel, MINIMUM_LEVEL);
    //// RELEASE
    // WHEN
    _adsrEnv.release();
    val = advanceEnv(_adsrEnv, _parameters.releaseTime);
    EXPECT_NEAR(val, 0.0, MINIMUM_LEVEL);
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
    EXPECT_NEAR(buffer[buffer.size() - 1], 1.0, MINIMUM_LEVEL);
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
    EXPECT_TRUE(_adsrEnv.isInState<Idle>());
}

TEST_F(ADSRTest, releaseWhenInAttack) {
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime / 2);
    _adsrEnv.release();
    EXPECT_TRUE(_adsrEnv.isInState<Release>());
    advanceEnv(_adsrEnv, _parameters.releaseTime);
    EXPECT_TRUE(_adsrEnv.isInState<Idle>());
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime / 2);
    EXPECT_TRUE(_adsrEnv.isInState<Attack>());
}


TEST_F(ADSRTest, releaseWhenInDecay) {
    // GIVEN
    double val = 0.0;
    double minimumLevel = 0.0001;
    auto samplesAfterRelease = static_cast<uint32_t>(0.5 * _samplingFreq_hz);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(samplesAfterRelease * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, samplesAfterRelease);

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv, _parameters.attackTime + (_parameters.decayTime / 2));
    _adsrEnv.release();
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferDecreasing(bufferToFill));
    EXPECT_TRUE(!_adsrEnv.isActive());
    EXPECT_NEAR(_adsrEnv.tick(), minimumLevel, 0.0001);
}

TEST_F(ADSRTest, AttackWhenInDecay) {
    const uint32_t nChannels = 2;
    const double attackStartValue = 0.001;
    const double attackEndValue = 1.0;
    _adsrEnv.trigger();
    auto val = advanceEnv(_adsrEnv, _parameters.attackTime + (_parameters.decayTime / 2));
    _adsrEnv.trigger();
    auto attackSamplesLeft = getSectionSamplesLeft(attackStartValue, attackEndValue, _samplingFreq_hz,
                                                   _parameters.attackTime, val);
    std::vector<float> buffer(static_cast<size_t>(attackSamplesLeft) * nChannels);
    std::fill(buffer.begin(), buffer.end(), 1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(), nChannels, attackSamplesLeft);
    _adsrEnv.process(bufferToFill);
    ASSERT_TRUE(isBufferIncreasing(bufferToFill));
}

TEST_F(ADSRTest, AttackWhenInSustain) {

}

TEST_F(ADSRTest, AttackWhenInRelease) {

}


