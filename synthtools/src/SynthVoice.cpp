#include "SynthVoice.h"
#include "Conversions.h"

namespace synthtools {

SynthVoice::SynthVoice(float sampleRate, uint32_t nChannels,
                       utilities::AudioBufferTools::OscillatorType oscType)
     {
   _audioPipeline.getProcessor<dsp::BiquadFilter>().setUp(
           dsp::BiquadFilter::FilterSettings{
                   sampleRate, 0.707f, 200.0, dsp::BiquadFilter::Type::HIGHPASS, nChannels}
           );
   _audioPipeline.getProcessor<WavetableOsc>().setUp(
           oscType, 1000U, sampleRate, true
           );
   _audioPipeline.getProcessor<dsp::AdsrEnv>().setSampleRate(sampleRate);-
}

void SynthVoice::noteOn(uint8_t pitch, uint8_t velocity) {
  auto freq = utilities::Conversions::pitchToFrequency(pitch);
  _audioPipeline.getProcessor<WavetableOsc>().setFrequency(static_cast<float>(freq));
  _audioPipeline.getProcessor<dsp::AdsrEnv>().trigger();
}

void SynthVoice::noteOff(uint8_t pitch) {
    _audioPipeline.getProcessor<dsp::AdsrEnv>().release();
}

void SynthVoice::process(utilities::AudioBuffer<float> &buffer) {
  _audioPipeline.processAll(buffer);
}

void SynthVoice::setFilterEnabled(bool enable) {
    // TODO : SET FILTER BYPASS IN THE PIPELINE
    _filterEnabled = enable;
}

} // namespace synthtools