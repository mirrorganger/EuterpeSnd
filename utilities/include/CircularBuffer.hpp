#pragma once
#include <array>
#include <vector>
#include <cassert>

#include "Interpolation.h"

namespace utilities {

template <typename T> constexpr T power_ceil(T x) {
  if (x <= 1)
    return 1;
  int power = 2;
  x--;
  while (x >>= 1)
    power <<= 1;
  return power;
}

template <typename T> constexpr bool is_powerof2(T v) {
  return v && ((v & (v - 1)) == 0);
}

template <typename T>
void initialize_buffer(std::size_t size, std::vector<T> &buffer,
                       std::size_t &mask) {
  std::size_t required_size = power_ceil(size);
  mask = required_size - 1;
  buffer.resize(required_size, T{});
}

template <typename T, std::size_t N>
void initialize_buffer(std::array<T, N> &buffer, std::size_t &mask) {
  static_assert(is_powerof2(N), "Size must a power of two");
  mask = buffer.size() - 1;
}

template <typename T, typename BufferT = std::vector<T>> 
class CircularBuffer {
public:
  using index_type = std::size_t;

  explicit CircularBuffer() {
    initialize_buffer(_buffer, _mask);
  }

  explicit CircularBuffer(std::size_t size){
      initialize_buffer(size, _buffer, _mask);
  }

  CircularBuffer(CircularBuffer const &rhs) = default;
  CircularBuffer(CircularBuffer &&rhs) = default;

  CircularBuffer &operator=(CircularBuffer const &rhs) = default;
  CircularBuffer &operator=(CircularBuffer &&rhs) = default;

  T const &operator[](std::size_t index) const{
    return _buffer[(_writePos + index) & _mask];
  }

  T &operator[](std::size_t index){
    return _buffer[(_writePos + index) & _mask];
  }
  std::size_t size() const {
    return _buffer.size();
  }

  // Push a value into the buffer
  void push(T val) {
    --_writePos &= _mask;
    _buffer[_writePos] = val;
  }
  
  // Returns the latest element.
  T const &front() const{
    return (*this)[0];
  }
    
  T &front() {
    return (*this)[0];
  }

  // Returns the oldest element.
  T const &back() const {
    return (*this)[size() - 1];
  }

  T &back() {
    return (*this)[size() - 1];
  }

  // Clears the buffer
  void clear() {
    for (auto &element : _buffer) {
      element = T{};
    }
    _writePos = 0;
  }

  // Remove the latest element
  void pop_front() {
    _writePos++;
    _writePos &= _mask; // Ensure write position wraps around
  }
  

private:
  std::size_t _mask;
  std::size_t _writePos = 0;
  BufferT _buffer;
};

template <typename T, typename BufferT = std::vector<T>,
          typename IndexType = float>
class CircularBufferFrac : public CircularBuffer<T, BufferT> {
public:
  using base_type = CircularBuffer<T, BufferT>;
  using index_type = IndexType;

  using CircularBuffer<T, BufferT>::CircularBuffer;

  T operator[](IndexType index) const {
    linear_interpolation interpolation;
    return interpolation(static_cast<base_type const &>(*this), index);
  }
};


template <typename DataType>
class MultiChannelBuffer {
public:
  MultiChannelBuffer(std::size_t numChannels, std::size_t bufferSize)
      : _numChannels(numChannels) {
    std::size_t _bufferSize = power_ceil(bufferSize);
    _buffer.resize(numChannels * _bufferSize);
    _writePos.resize(numChannels, 0U);
    _mask = _bufferSize - 1;
  }

  void push(DataType value, std::size_t channel) {
    assert(channel < _numChannels && "Channel index out of range");
    auto channelWritePos = --_writePos[channel] & _mask;
    _buffer[channel + channelWritePos * _numChannels] = value;
    _writePos[channel] = channelWritePos;
  }

  DataType operator()(std::size_t channel, std::size_t index) const {
   assert(channel < _numChannels && "Channel index out of range");
   auto channelOffset = (_writePos[channel] + index) & _mask;
   return _buffer[channel + channelOffset * _numChannels];
  }


private:
  std::vector<DataType> _buffer;
  std::vector<std::size_t> _writePos;
  std::size_t _numChannels;
  std::size_t _bufferSize;
  std::size_t _mask;
};



} // namespace utilities
