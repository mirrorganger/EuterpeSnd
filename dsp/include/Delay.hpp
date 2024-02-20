#pragma once

#include "CircularBuffer.hpp"

namespace dsp {

template <typename BaseType> class DelayT : public BaseType {
public:
  using index_type = typename BaseType::index_type;

  DelayT(float delay_ms, float sampleRate)
      : BaseType(std::size_t(delay_ms * sampleRate)) {}

  explicit DelayT(float delayInSamples)
      : BaseType(std::size_t(delayInSamples)) {}

  // returns maximum delay
  float operator()() const { return this->back(); }

  // returns delay at index
  float operator()(index_type index) const { return (*this)[index];}

};

using FractionalDelay = DelayT<utilities::CircularBufferFrac<float>>;
using Delay = DelayT<utilities::CircularBuffer<float>>;

} // namespace dsp