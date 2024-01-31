#include "DelayLine.h"

#include <gmock/gmock.h>

#include <numeric>

using namespace dsp;
using namespace testing;
#include <ctime>
#include <cstdlib>
namespace {

    TEST(TestDelayLine, testGetSamples){
        constexpr std::size_t maxDelay = 10;
        DelayLine<uint32_t, maxDelay> delayLine;
        std::array<uint32_t, maxDelay> data;
        std::iota(data.begin(),data.end(),0);
        delayLine.clear();

        for(const auto & value : data) {
            delayLine.write(value);
        }

        for (size_t delay = 0; delay < maxDelay; delay++)
        {
            delayLine.setDelay(delay);
            EXPECT_EQ(delayLine.read(),((maxDelay-delay-1) % maxDelay));
        }
    }

    TEST(TestDelayLine, testFractionalDelay){
        srand( (unsigned)time( NULL ) );
        constexpr std::size_t maxDelay = 10;
        DelayLine<float, maxDelay> delayLine;
        delayLine.clear();

        for (int i = 0; i < maxDelay ; ++i) {
            delayLine.write(static_cast<float>(i));
        }
        float delay = 1.0f + static_cast<float>(rand())/static_cast<float>(RAND_MAX);
        while(static_cast<std::size_t>(delay) < (maxDelay-1)){
            delayLine.setDelay(delay);
            EXPECT_FLOAT_EQ(delayLine.read(),static_cast<float>(maxDelay-1)-delay);
            delay+=static_cast<float>(rand())/static_cast<float>(RAND_MAX);
        }
    }
}



