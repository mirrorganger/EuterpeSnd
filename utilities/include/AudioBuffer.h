#ifndef EUTERPESND_UTILITIES_AUDIOBUFFER_H
#define EUTERPESND_UTILITIES_AUDIOBUFFER_H

#include <cstdint>
#include <stdlib.h>

namespace utilities
{

template<typename DataType>
class AudioBuffer{
public:
    AudioBuffer(DataType*  bufferToUse, uint32_t numberOfChannels, uint32_t framesPerBuffer):
        _data(bufferToUse),
        _nChannels(numberOfChannels),
        _framesPerBuffer(framesPerBuffer)       
    {

    }

    DataType * getWritePointer(){
        return _data;
    }    

    DataType& operator[] (size_t n){
        return _data[n];    
    }

    DataType operator[] (size_t n) const{
        return _data[n];    
    }

    uint32_t getNumberOfChannels(){
        return _nChannels;
    }

    uint32_t getFramesPerBuffer(){
        return _framesPerBuffer;
    }

private:
    DataType * _data; 
    uint32_t _nChannels;
    uint32_t _framesPerBuffer;
};



template<typename DataType>
class AudioProcessor 
{
public:
    virtual void process(AudioBuffer<DataType>& buffer) = 0;
    virtual ~AudioProcessor() = default;
};



}

#endif