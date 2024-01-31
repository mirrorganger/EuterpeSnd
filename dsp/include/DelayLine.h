#pragma once

#include <array>

namespace dsp {

    template<typename DataType, std::size_t MAX_DELAY_SIZE>
    class DelayLine {
    public:
        void clear() {
            std::fill(_buffer.begin(), _buffer.end(), DataType{});
            _writePtr = 0;
            _delayInSamples = 0;
        }

        void setDelay(std::size_t delayInSamples) {
            _delayInSamples = delayInSamples < MAX_DELAY_SIZE ? delayInSamples : MAX_DELAY_SIZE - 1U;
        }

        void setDelay(float delayInSamples) {
            auto integerDelay = static_cast<std::size_t>(delayInSamples);
            _frac = delayInSamples - static_cast<float>(integerDelay);
            _delayInSamples =
                    static_cast<std::size_t>(integerDelay) < MAX_DELAY_SIZE ? integerDelay : MAX_DELAY_SIZE - 1U;
        }

        void write(DataType value) {
            _buffer[_writePtr] = value;
            _writePtr = (++_writePtr) % MAX_DELAY_SIZE;
        }

        DataType read() const {
            auto lastDataIndex = (_writePtr - 1U);
            DataType current = _buffer[(lastDataIndex - _delayInSamples + MAX_DELAY_SIZE) % MAX_DELAY_SIZE];
            DataType prev = _buffer[(lastDataIndex - _delayInSamples - 1U + MAX_DELAY_SIZE) % MAX_DELAY_SIZE];
            return current + (prev - current) * _frac;
        }

    private:
        std::size_t _writePtr = {};
        std::size_t _delayInSamples = {};
        std::array<DataType, MAX_DELAY_SIZE> _buffer;
        float _frac = {};
    };

} // namespace dsp
