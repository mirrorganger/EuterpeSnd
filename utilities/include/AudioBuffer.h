#ifndef EUTERPESND_UTILITIES_AUDIOBUFFER_H
#define EUTERPESND_UTILITIES_AUDIOBUFFER_H

#include <cstdint>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>

namespace utilities {

    template<typename DataType>
    class AudioBuffer {
    public:
        AudioBuffer(DataType *bufferToUse, uint32_t numberOfChannels, uint32_t framesPerBuffer) :
                _data(bufferToUse),
                _nChannels(numberOfChannels),
                _framesPerBuffer(framesPerBuffer) {

        }

        [[nodiscard]] DataType *getWritePointer() {
            return _data;
        }

        [[nodiscard]] DataType *getReadPointer() const {
            return _data;
        }

        [[nodiscard]] DataType &operator[](size_t n) {
            return _data[n];
        }

        [[nodiscard]] DataType operator[](size_t n) const {
            return _data[n];
        }

        [[nodiscard]] uint32_t getNumberOfChannels() const {
            return _nChannels;
        }

        [[nodiscard]] uint32_t getFramesPerBuffer() const {
            return _framesPerBuffer;
        }

    private:
        DataType *_data;
        uint32_t _nChannels;
        uint32_t _framesPerBuffer;
    };


    template<typename DataType>
    class AudioProcessor {
    public:
        virtual void process(AudioBuffer<DataType> &buffer) = 0;

        virtual ~AudioProcessor() = default;
    };

    template<typename DataType, size_t N = 1U>
    class MonoBuffer : public AudioBuffer<DataType> {
        MonoBuffer(DataType *bufferToUse, uint32_t framesPerBuffer) : AudioBuffer<DataType>(bufferToUse, N,
                                                                                            framesPerBuffer) {}
    };

    template<typename DataType, size_t N = 1>
    void monoToMultipleChannels(const MonoBuffer<DataType> &monoBuffer, AudioBuffer<DataType> &stereoBuffer) {
        static_assert(monoBuffer.getNumberOfChannels() == N);
        auto dataPtr = stereoBuffer.getWritePointer();
        for (uint32_t sample_i = 0; sample_i < monoBuffer.getFramesPerBuffer(); ++sample_i) {
            for (uint32_t channel_i = 0; channel_i < stereoBuffer.getNumberOfChannels(); ++channel_i) {
                *dataPtr++ = monoBuffer[sample_i];
            }
        }
    }

    template<typename DataType>
    DataType getChannelMag(const AudioBuffer<DataType>& buffer, uint32_t startSample, uint32_t numSamples, uint32_t channel){
        auto readPtr = buffer.getReadPointer();
        auto channelMag = std::numeric_limits<DataType>::min();
        for (uint32_t i = startSample; i < startSample+numSamples; i++)
        {
            channelMag = std::max(readPtr[i*buffer.getNumberOfChannels()+channel],channelMag);
        }
        return channelMag;
    }


}
#endif