#include "AdsrEnv.h"

#include <gmock/gmock.h>

#include <iostream>

using namespace dsp;
using namespace testing;

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
static bool isBufferDecreasing(const utilities::AudioBuffer<T>& buffer){
    for (uint32_t channel = 0; channel < buffer.getNumberOfChannels() ; ++channel) {
        T prevSample = std::numeric_limits<T>::max();
        for (int sample = 0; sample < buffer.getFramesPerBuffer() ; ++sample) {
            auto currentSample = buffer[sample * buffer.getNumberOfChannels() + channel];
            if(currentSample >= prevSample){
                std::cout << currentSample << ", " << prevSample;
                return false;
            }
            prevSample = currentSample;
        }
    }
    return true;
}

template<typename T>
static bool isBufferIncreasing(const utilities::AudioBuffer<T>& buffer){
    for (uint32_t channel = 0; channel < buffer.getNumberOfChannels() ; ++channel) {
        T prevSample = std::numeric_limits<T>::min();
        for (int sample = 0; sample < buffer.getFramesPerBuffer() ; ++sample) {
            auto currentSample = buffer[sample * buffer.getNumberOfChannels() + channel];
            if(currentSample <= prevSample){
                std::cout << currentSample << ", " << prevSample;
                return false;
            }
            prevSample = currentSample;
        }
    }
    return true;
}

template<typename T>
utilities::AudioBuffer<T> getAudioBuffer(double sampleRate, double lengthInSecs)
{
    auto lengthInSamples = static_cast<uint64_t>(sampleRate * lengthInSecs);

}


class ADSRTest : public Test {
public:
    ADSRTest()
    {
        _parameters.attackTimeSec = 1.0;
        _parameters.decayTimeSec = 1.0;
        _parameters.releaseTimeSec = 1.0;
        _parameters.sustainLevel = 0.6;
        _adsrEnv.configure(_parameters);
        _adsrEnv.setSampleRate(_samplingFreq);
    }

    static double advanceEnv(AdsrEnv& env, uint64_t numTicks){
        double val = 0.0;
        for (uint64_t i = 0U; i < numTicks; ++i) {
            val = env.tick();
        }
        return val;
    }

protected:
    AdsrEnv _adsrEnv;
    double _samplingFreq = 100.0;
    AdsrEnv::Parameters _parameters;
};

TEST_F(ADSRTest, initialNoActive) {
    EXPECT_TRUE(!_adsrEnv.isActive());
    for (int times = 0; times < 100; ++times) {
        EXPECT_FLOAT_EQ(_adsrEnv.tick(), 0.0f);
    }
}

TEST_F(ADSRTest, completeAnEnvelope){
    // GIVEN
    auto ticksAttack = static_cast<uint64_t>(_parameters.attackTimeSec * _samplingFreq);
    auto ticksDecay = static_cast<uint64_t>(_parameters.decayTimeSec * _samplingFreq);
    auto ticksRelease = static_cast<uint64_t>(_parameters.releaseTimeSec * _samplingFreq);
    double val = 0.0;
    double minimumLevel = 0.0001;
    //// TRIGGER
    // WHEN
    _adsrEnv.trigger();
    // THEN
    ASSERT_TRUE(_adsrEnv.isActive());
    //// ATTACK
    //WHEN
    val = advanceEnv(_adsrEnv,ticksAttack);
    // THEN
    EXPECT_FLOAT_EQ(val,1.0);
    //// SUSTAIN
    // WHEN
    val = advanceEnv(_adsrEnv,ticksDecay);
    // THEN
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);
    //// SUSTAIN
    val = advanceEnv(_adsrEnv,1000U);
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);
    //// RELEASE
    // WHEN
    _adsrEnv.release();
    val = advanceEnv(_adsrEnv,ticksRelease);
    // THEN
    // EXPECT_FLOAT_EQ(val,minimumLevel);
    //// IDLE
    EXPECT_TRUE(!_adsrEnv.isActive());
}


TEST_F(ADSRTest, Attack){
    // GIVEN
    auto ticksAttack = static_cast<int64_t>(_parameters.attackTimeSec * _samplingFreq);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(ticksAttack*nChannels);
    std::fill(buffer.begin(), buffer.end(),1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(),nChannels,ticksAttack);
    // WHEN
    _adsrEnv.trigger();
    // THEN
    ASSERT_TRUE(_adsrEnv.isActive());
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferIncreasing(bufferToFill));
    EXPECT_FLOAT_EQ(buffer[buffer.size()-1],1.0);    
}

TEST_F(ADSRTest, Decay){
    // GIVEN
    auto tickDecay = static_cast<int64_t>(_parameters.decayTimeSec * _samplingFreq);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(tickDecay*nChannels);
    std::fill(buffer.begin(), buffer.end(),1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(),nChannels,tickDecay);

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv,static_cast<int64_t>(_parameters.attackTimeSec * _samplingFreq));
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferDecreasing(bufferToFill));
    EXPECT_FLOAT_EQ(buffer[buffer.size()-1],_parameters.sustainLevel);    
    
}

TEST_F(ADSRTest, Sustain){
    // GIVEN
    auto numberOfticks = static_cast<int64_t>(2.0 * _samplingFreq);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(numberOfticks*nChannels);
    std::vector<float> expectedBuffer(numberOfticks*nChannels);
    std::fill(buffer.begin(), buffer.end(),1.0);
    std::fill(expectedBuffer.begin(), expectedBuffer.end(), _parameters.sustainLevel);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(),nChannels,numberOfticks);

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv,static_cast<int64_t>((_parameters.attackTimeSec + _parameters.decayTimeSec) * _samplingFreq));
    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    EXPECT_EQ(buffer,expectedBuffer);
}



TEST_F(ADSRTest, Release){
    // GIVEN
    auto ticksRelease = static_cast<int64_t>(_parameters.releaseTimeSec * _samplingFreq);
    const uint32_t nChannels = 2U;
    std::vector<float> buffer(ticksRelease*nChannels);
    std::fill(buffer.begin(), buffer.end(),1.0);
    utilities::AudioBuffer<float> bufferToFill(buffer.data(),nChannels,ticksRelease);
    double minimumLevel = 0.0001;

    // WHEN
    _adsrEnv.trigger();
    advanceEnv(_adsrEnv,static_cast<int64_t>((_parameters.attackTimeSec + _parameters.decayTimeSec) * _samplingFreq));
    auto val = advanceEnv(_adsrEnv,100U);
    EXPECT_FLOAT_EQ(val,_parameters.sustainLevel);
    _adsrEnv.release();

    // WHEN
    _adsrEnv.process(bufferToFill);
    // THEN
    ASSERT_TRUE(isBufferDecreasing(bufferToFill));
}




