#include "CircularBuffer.hpp"
#include <array>
#include <iterator>
#include <random>
#include <algorithm>


#include <gmock/gmock.h>

template<typename T, size_t N>
std::array<T, N> generate_random_array(T min, T max){
    static std::random_device rd;    // you only need to initialize it once
    static std::mt19937 mte(rd());   // this is a relative big object to create

    std::uniform_int_distribution<T> dist(min, max);

    std::array<T,N> a;
    std::generate(a.begin(), a.end(), [&] () { return dist(mte); });
    return a;
}

TEST(CircularBufferTests, simpleOperations)
{

    constexpr std::size_t N = 128;
    utilities::CircularBuffer<int32_t,std::array<int32_t,N>> c_buffer;
    
    auto samples = generate_random_array<int32_t,N>(30,450);

    for (const auto& sample : samples)
    {
        c_buffer.push(sample);
    }
    
   // Get oldest sample
    EXPECT_EQ(c_buffer.back(),samples.front());    
    // Get newests sample
    EXPECT_EQ(c_buffer.front(),samples.back());

    for (size_t i = 0; i < c_buffer.size(); i++)
    {
        EXPECT_EQ(c_buffer[i],samples[samples.size()-1-i]);
    }
    
    c_buffer.pop_front();
    EXPECT_EQ(c_buffer.front(),samples[samples.size()-2]);    
}

TEST(CircularBufferTests, simpleOperationsVector)
{   
    constexpr std::size_t N = 64;
    utilities::CircularBuffer<int32_t,std::vector<int32_t>> c_buffer(N);
    
    auto samples = generate_random_array<int32_t,N>(312,42950);

    for (const auto& sample : samples)
    {
        c_buffer.push(sample);
    }
    
   // Get oldest sample
    EXPECT_EQ(c_buffer.back(),samples.front());    
    // Get newests sample
    EXPECT_EQ(c_buffer.front(),samples.back());

    for (size_t i = 0; i < c_buffer.size(); i++)
    {
        EXPECT_EQ(c_buffer[i],samples[samples.size()-1-i]);
    }
    
    c_buffer.pop_front();
    EXPECT_EQ(c_buffer.front(),samples[samples.size()-2]);    
}





