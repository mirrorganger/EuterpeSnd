#include "SynthVoice.h"
#include "Conversions.h"

namespace synthtools{

SynthVoice::SynthVoice(float sampleRate) :
        _wavetableOsc(utilities::AudioBufferTools::OscillatorType::SINE,1000U,sampleRate,true)
{
    _env.setSampleRate(sampleRate);
}

void SynthVoice::noteOn(uint8_t pitch, uint8_t velocity) {
    auto freq = utilities::Conversions::pitchToFrequency(pitch);
    _wavetableOsc.setFrequency(freq);
    _env.trigger();
}

void SynthVoice::noteOff(uint8_t pitch) {
    _env.release();
}

void SynthVoice::process(utilities::AudioBuffer<float> &buffer) {
    // Do all operations with just one buffer?
    // Replicate process
    _wavetableOsc.process(buffer);

}

}