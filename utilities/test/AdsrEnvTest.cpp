#include "AdsrEnv.h"

#include <gmock/gmock.h>

#include <iostream>

using namespace utilities;
using namespace testing;

class ADSRTest : public Test {
public:
    ADSRTest()
    {
        mParameters.attackTimeSec = 1.0f;
        mParameters.decayTimeSec = 1.0f;
        mParameters.releaseTimeSec = 1.0f;
        mParameters.sustainLevel = 0.6;;
        mAdsr.configure(mParameters);
        mAdsr.setSampleRate(mSamplingFreq);
    }

protected:
    AdsrEnv mAdsr;
    float mSamplingFreq = 44100.0;
    AdsrEnv::Parameters mParameters;
};

TEST_F(ADSRTest, checkInitialState) {
    EXPECT_TRUE(!mAdsr.isActive());
    EXPECT_FLOAT_EQ(mAdsr.tick(), 0.0f);
}

TEST_F(ADSRTest, checkNormalProcess){
    // GIVEN
    int64_t ticksAttack = static_cast<int>(mParameters.attackTimeSec * mSamplingFreq);
    int64_t ticksDecay = static_cast<int>(mParameters.decayTimeSec * mSamplingFreq);
    int64_t ticksRelease = static_cast<int>(mParameters.releaseTimeSec * mSamplingFreq);
    float val = 0.0;

    // WHEN
    mAdsr.trigger();
    // THEN
    ASSERT_TRUE(mAdsr.isActive());

    // WHEN
    while(ticksAttack-- >= 0U){
        val = mAdsr.tick();
    }
    // THEN
    EXPECT_FLOAT_EQ(val,1.0f);

    // WHEN
    while(ticksDecay-- >= 0U){
        val = mAdsr.tick();
    }
    // THEN
    EXPECT_FLOAT_EQ(val,mParameters.sustainLevel);

    // WHEN
    mAdsr.release();
    while(ticksRelease-- >= 0U){
        val = mAdsr.tick();
    }
    // THEN
    EXPECT_FLOAT_EQ(val,.0f);
    EXPECT_TRUE(!mAdsr.isActive());
}


