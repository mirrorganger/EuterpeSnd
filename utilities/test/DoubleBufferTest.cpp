#include "DoubleBuffer.h"

#include <string>

#include <gmock/gmock.h>

using namespace utilities;
using namespace testing;

TEST(TestDelayLine, testGetSample) {
  constexpr std::size_t bufferSize = 4096U;
  std::array<int16_t, bufferSize * 2> expectedBuffer;

  for (size_t i = 0; i < expectedBuffer.size(); i++) {
    expectedBuffer[i] = i;
  }

  DoubleBuffer<int16_t, bufferSize> buffer;
  // Loading low part
  {
    auto dataBuffer = buffer.getWriteBuffer();
    std::copy(expectedBuffer.begin(), expectedBuffer.begin() + bufferSize,
              dataBuffer.begin());
  }

  // Reading low part
  for (size_t i = 0; i < bufferSize; i++) {
    EXPECT_EQ(buffer.readData(), expectedBuffer[i]);
  }

  // {
  //  auto dataBuffer = buffer.getWriteBuffer();
  //  std::copy(expectedBuffer.begin()+dataBuffer.size(),expectedBuffer.end(),dataBuffer.begin());
  // }

  // // Reading low part
  // for (size_t i = bufferSize; i < 2*bufferSize; i++)
  // {
  //     EXPECT_EQ(buffer.readData(),expectedBuffer[i]);
  // }
}
