#pragma once

#include "Delay.hpp"
#include "Lfo.h"
namespace dsp {

    template <typename DataType> class ModulatedDelay {
    public:
        ModulatedDelay(float sampleRate_hz, float lfoFreq_hz, float delay_ms,
                       float lfoDepth, float feedback,  Lfo::WaveFormT type = Lfo::WaveFormT::TRIANGLE)
                : _lfo(sampleRate_hz, lfoFreq_hz, Lfo::WaveFormT::TRIANGLE),
                  _sampleRate_hz(sampleRate_hz), _baseDelay_ms(delay_ms), _lfoDepth(lfoDepth),_feedback(feedback) {
            for (auto delay : _delays) {
                delay.clear();
            }
        }

        void prepare(float sampleRate_hz,float lfoFreq_hz, size_t samplesPerBlock){
            _modulationValues.clear();
            _modulationValues.resize(samplesPerBlock);
            _sampleRate_hz = sampleRate_hz;
            for (auto delay : _delays) {
                delay.clear();
            }
            _lfo.reset(_sampleRate_hz,lfoFreq_hz);
        }

        void process(DataType * const * buffer, size_t numberOfChannels, size_t numberOfSamples) {
            auto out = DataType{};
            _lfo.getBlock(_modulationValues.data(),_modulationValues.size());
            std::ranges::for_each(_modulationValues,[&](float& val){
                auto modulation = std::max(1.0F,(_maxModulation_ms * val * _lfoDepth  + _baseDelay_ms));
                val = modulation * _sampleRate_hz / 1000.0F;
            });

            for (size_t channel_i = 0; channel_i < _delays.size(); ++channel_i) {
                for (size_t sample_i = 0; sample_i < numberOfSamples; ++sample_i) {
                    auto in = buffer[channel_i][sample_i];
                    DataType yn = _delays[channel_i](_modulationValues[sample_i]);
                    DataType dn = in + _feedback * yn;
                    _delays[channel_i].push(dn);
                    buffer[channel_i][sample_i] = _dryWetMix * in + (1.0F-_dryWetMix) * dn;
                }
            }
        }

        void setDelay(float delay_ms){
            _baseDelay_ms = delay_ms;
        }

        void setFeedback(float feedback) {
            _feedback = feedback;
        }
        void setDryWetMix(float dryWet){
            _dryWetMix = dryWet;
        }

        void setLfoDepth(float lfoDepth){
            _lfoDepth = lfoDepth;
        }

        void setLfoFreq(float lfoFreq){
            _lfo.update(_sampleRate_hz,lfoFreq);
        }
    private:
        constexpr static std::size_t max_delay_samples = 2 * 44'100; // 2s.
        std::array<dsp::FractionalDelay ,2U> _delays = {
                dsp::FractionalDelay(max_delay_samples),
                dsp::FractionalDelay(max_delay_samples)
        };
        std::vector<float> _modulationValues;
        Lfo _lfo;
        float _sampleRate_hz = 44100.0F;
        float _baseDelay_ms = 10.0;
        float _maxModulation_ms = 4.0;
        float _lfoDepth = 1.0F;
        float _feedback = 0.5F;
        float _dryWetMix = 0.9F;
    };

} // namespace dsp


