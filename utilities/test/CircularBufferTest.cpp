#include "CircularBuffer.hpp"
#include <algorithm>
#include <array>
#include <iterator>
#include <random>

#include <gmock/gmock.h>

template <typename T, size_t N>
std::array<T, N> generateRandomArray(uint32_t min, uint32_t max) {
  static std::random_device rd;  // you only need to initialize it once
  static std::mt19937 mte(rd()); // this is a relative big object to create

  std::uniform_int_distribution<uint64_t> dist(min, max);

  std::array<T, N> a;
  std::ranges::generate(a, [&]() { return static_cast<T>(dist(mte)); });
  return a;
}

template <size_t N, typename DataType, typename StorageT>
void verifyBuffer(utilities::CircularBuffer<DataType, StorageT> &c_buffer) {

  auto samples = generateRandomArray<DataType, N>(30, 450);

  for (const auto &sample : samples) {
    c_buffer.push(sample);
  }

  // Get oldest sample
  EXPECT_EQ(c_buffer.back(), samples.front());
  // Get newest sample
  EXPECT_EQ(c_buffer.front(), samples.back());

  for (size_t i = 0; i < c_buffer.size(); i++) {
    EXPECT_EQ(c_buffer[i], samples[samples.size() - 1 - i]);
  }

  c_buffer.pop_front();
  EXPECT_EQ(c_buffer.front(), samples[samples.size() - 2]);
}

TEST(CircularBufferTests, simpleOperations) {
  {
    constexpr std::size_t N = 1 << 4;
    utilities::CircularBuffer<int32_t, std::array<int32_t, N>> c_buffer;
    verifyBuffer<N>(c_buffer);
  }

  {
    constexpr std::size_t N = 1 << 8;
    utilities::CircularBuffer<int64_t, std::vector<int64_t>> c_buffer(N);
    verifyBuffer<N>(c_buffer);
  }
}

TEST(MultiCircularBufferTets, simpleOperations) {
  {
    constexpr std::size_t N = 1 << 6;
    constexpr std::size_t numChannels = 4;
    utilities::MultiChannelBuffer<int32_t> mc_buffer(numChannels, N);
    auto samples = generateRandomArray<int32_t, N>(30, 450);

    for(size_t channel = 0; channel < numChannels; ++channel) {
      for (const auto &sample : samples) {
        mc_buffer.push(sample*(channel+1), channel);
      }
    }
    
    for (size_t channel = 0; channel < numChannels; ++channel) {
      for (size_t i = 0; i < N; ++i) {
        auto expected_value = samples[N - 1 - i] * (channel + 1);
        auto value = mc_buffer(channel, i);
        EXPECT_EQ(value, expected_value) << "Channel: " << channel << ", Index:" << i;
      }
    }


  }

}


