#include "AdsrEnv.h"

#include <gmock/gmock.h>

#include <iostream>

using namespace dsp;
using namespace testing;

class ADSRTest : public Test {
public:
    ADSRTest()
    {
        _parameters.attackTimeSec = 1.0f;
        _parameters.decayTimeSec = 1.0f;
        _parameters.releaseTimeSec = 1.0f;
        _parameters.sustainLevel = 0.6;;
        _adsrEnv.configure(_parameters);
        _adsrEnv.setSampleRate(_samplingFreq);
    }

protected:
    AdsrEnv _adsrEnv;
    float _samplingFreq = 44100.0;
    AdsrEnv::Parameters _parameters;
};

TEST_F(ADSRTest, checkInitialState) {
    EXPECT_TRUE(!_adsrEnv.isActive());
    EXPECT_FLOAT_EQ(_adsrEnv.tick(), 0.0f);
}

TEST_F(ADSRTest, checkNormalProcess){
    // GIVEN
    auto ticksAttack = static_cast<int64_t>(_parameters.attackTimeSec * _samplingFreq);
    

    auto ticksDecay = static_cast<int64_t>(_parameters.decayTimeSec * _samplingFreq);
    auto ticksRelease = static_cast<int64_t>(_parameters.releaseTimeSec * _samplingFreq);
    float val = 0.0;

    // WHEN
    _adsrEnv.trigger();
    // THEN
    ASSERT_TRUE(_adsrEnv.isActive());

    // WHEN
    // while(ticksAttack-- < 1000U){
    //     val = _adsrEnv.tick();
    // }

    //WHEN
    while(val != 1.0){
        val = _adsrEnv.tick();
        ticksAttack--;
    }
    // THEN
    EXPECT_FLOAT_EQ(val,1.0f);

    // WHEN
    while(val != _parameters.sustainLevel){
        val = _adsrEnv.tick();
        ticksDecay--;
    }
    // THEN
    EXPECT_FLOAT_EQ(val, _parameters.sustainLevel);

    // WHEN
    _adsrEnv.release();
    while(ticksRelease-- >= 0U){
        val = _adsrEnv.tick();
    }
    // THEN
    EXPECT_FLOAT_EQ(val,.0f);
    EXPECT_TRUE(!_adsrEnv.isActive());
}


