#ifndef EUTERPESND_SYNTHVOICE_H
#define EUTERPESND_SYNTHVOICE_H

#include "AudioBuffer.h"
#include "WavetableOsc.h"
#include "AdsrEnv.h"

#include <cstdint>

namespace  synthtools
{
    class SynthVoice : public utilities::AudioProcessor<float> {
    public:
        SynthVoice(float sampleRate,utilities::AudioBufferTools::OscillatorType oscType = utilities::AudioBufferTools::OscillatorType::SINE);
        void noteOn(uint8_t pitch, uint8_t velocity);
        void noteOff(uint8_t pitch);
        void process(utilities::AudioBuffer<float>& buffer) override;
    private:
        WavetableOsc _wavetableOsc;
        dsp::AdsrEnv _env{};
        float _sampleRate_Hz;
        float _freq_Hz = 440.0f;
    };

}
#endif
