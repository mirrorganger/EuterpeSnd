#pragma once

#include <cstdint>
#include <array>
namespace utilities
{

template<typename DataType, std::size_t BufferSize>
class DoubleBuffer
{
    using SimpleBuffer = std::array<DataType,BufferSize>;
public:
    DataType readData(){
      DataType data = {};  
      data = (_readPtr > BufferSize)? _lowBuffer[_readPtr] : _highBuffer[_readPtr];    
      incrementReadPtr();
      return data;
    }

    [[nodiscard]] SimpleBuffer& getWriteBuffer(){
      return _loadingHigh? _lowBuffer : _highBuffer;
    }

private:
    void incrementReadPtr(){
        _readPtr = (_readPtr + 1U) % (BufferSize * 2);
        _loadingHigh = (_readPtr > BufferSize)? false : true; 
    }
    bool _loadingHigh = false;
    std::size_t _readPtr = 0U;
    std::array<DataType,BufferSize> _lowBuffer;
    std::array<DataType,BufferSize> _highBuffer;
};

    
} // namespace utilities


