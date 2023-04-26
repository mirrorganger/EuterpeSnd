#include "SynthVoice.h"
#include "Conversions.h"

namespace synthtools {

SynthVoice::SynthVoice(float sampleRate, uint32_t nChannels,
                       utilities::AudioBufferTools::OscillatorType oscType)
    : _wavetableOsc(oscType, 1000U, sampleRate, true) {
  _env.setSampleRate(sampleRate);
  _filter.setUp(dsp::BiquadFilter::FilterSettings{
      sampleRate, 0.707f, 200.0, dsp::BiquadFilter::Type::HIGHPASS, nChannels});
}

void SynthVoice::noteOn(uint8_t pitch, uint8_t velocity) {
  auto freq = utilities::Conversions::pitchToFrequency(pitch);
  _wavetableOsc.setFrequency(static_cast<float>(freq));
  _env.trigger();
}

void SynthVoice::noteOff(uint8_t pitch) { _env.release(); }

void SynthVoice::process(utilities::AudioBuffer<float> &buffer) {
  // Do all operations with just one buffer?
  // Replicate process
  _wavetableOsc.process(buffer);
  _env.process(buffer);
  if (_filterEnabled)
    _filter.process(buffer);
}

void SynthVoice::setFilterEnabled(bool enable) { _filterEnabled = enable; }

} // namespace synthtools