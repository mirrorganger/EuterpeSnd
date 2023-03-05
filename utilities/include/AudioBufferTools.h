#ifndef EUTERPESND_UTILITIES_AUDIOBUFFERTOOLS_H
#define EUTERPESND_UTILITIES_AUDIOBUFFERTOOLS_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

namespace utilities {

class AudioBufferTools {
  static constexpr uint8_t HIGHER_HARMONIC = 48;
  using MonoBuffer = std::vector<float>;

public:
  enum class OscillatorType : uint8_t {
    SINE,
    SAWTOOTH,
    TRIANGLE,
    SQUARE,
    FREE
  };

  static void makeOscTable(MonoBuffer &buffer, OscillatorType oscillatorType) {
    switch (oscillatorType) {
    case OscillatorType::SINE:
      utilities::AudioBufferTools::makeSineTable(buffer);
      break;
    case OscillatorType::SAWTOOTH:
      utilities::AudioBufferTools::makeSawToothTable(buffer);
      break;
    case OscillatorType::SQUARE:
      utilities::AudioBufferTools::makeSquareTable(buffer);
      break;
    case OscillatorType::TRIANGLE:
      utilities::AudioBufferTools::makeSineTable(buffer);
      break;
    default:
      break;
    }
  }

  static void makeTriangleTable(MonoBuffer &buffer) {
    std::generate(buffer.begin(), buffer.begin() + buffer.size() / 2,
                  [&, n = 0]() mutable {
                    return -1.0 + 4.0 * static_cast<float>(n++) /
                                      static_cast<float>(buffer.size());
                  });
    std::generate(buffer.begin() + buffer.size() / 2, buffer.end(),
                  [&, n = buffer.size() / 2]() mutable {
                    return 1.0 -
                           4.0 * static_cast<float>(n++ - buffer.size() / 2) /
                               static_cast<float>(buffer.size());
                  });
  }

  static void makeSineTable(MonoBuffer &buffer) {
    std::generate(buffer.begin(), buffer.end(), [&, n = 0]() mutable {
      return sinf(2.0 * M_PI * static_cast<float>(n++) /
                  static_cast<float>(buffer.size()));
    });
  }

  static MonoBuffer makeSineWave(double freq, double sampleRate, uint32_t numSamples){
    std::vector<float> buffer(numSamples);
    std::generate(buffer.begin(), buffer.end(), [&, n = 0]() mutable {
      return static_cast<float>(std::sin(2.0 * M_PIf64 * static_cast<double>(n++) * freq /
                  static_cast<double >(buffer.size())));
    });
    return buffer;
  }


  static void makeSquareTable(MonoBuffer &buffer) {
    std::fill(buffer.begin(), buffer.begin() + buffer.size() / 2, 1.0);
    std::fill(buffer.begin() + buffer.size() / 2, buffer.end(), -1.0);
  }

  static void makeSawToothTable(MonoBuffer &buffer) {
    std::generate(buffer.begin(), buffer.end(), [&, n = 0]() mutable {
      float val = 0.0;
      for (uint8_t harmonic = 1; harmonic <= HIGHER_HARMONIC; harmonic++) {
        val += sinf(2.0 * M_PI * static_cast<float>(harmonic) *
                    static_cast<float>(n) / static_cast<float>(buffer.size())) /
               static_cast<float>(harmonic);
      }
      n++;
      return val;
    });
  }

  static float interpolation(const MonoBuffer &buffer, float indexPtr) {
    int idxBelow = floorf(indexPtr);
    int idxAbove = idxBelow + 1;
    if (idxAbove >= buffer.size())
      idxAbove = 0;

    float weigthAbove = indexPtr - idxBelow;

    return (1.0 - weigthAbove) * buffer[idxBelow] +
           weigthAbove * buffer[idxAbove];
  }

  static void clearRawBuffer(float *buffer, uint32_t bufferSize) {
    std::memset(buffer, 0, sizeof(float) * bufferSize);
  }

  static void multiplyRawBuffer(float *buffer, uint32_t bufferSize,
                                float value) {
    for (std::size_t i = 0; i < bufferSize; ++i) {
      buffer[i] *= value;
    }
  }

  static void monoToStereo(const float *monoBuffer, float *stereoBuffer,
                           size_t numSamples, size_t numChannels) {
    for (size_t i = 0; i < numSamples; ++i) {
      for (size_t j = 0; j < numChannels; ++j) {
        *stereoBuffer++ = monoBuffer[i];
      }
    }
  }


};

} // namespace utilities

#endif // EUTERPESND_UTILITIES_AUDIOBUFFERTOOLS_H
