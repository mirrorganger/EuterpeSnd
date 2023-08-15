#ifndef EUTERPESND_SYNTHVOICE_H
#define EUTERPESND_SYNTHVOICE_H

#include "AdsrEnv.h"
#include "BiquadFilter.h"
#include "WavetableOsc.h"
#include "AudioPipeline.h"

#include <cstdint>

namespace synthtools {
class SynthVoice : public utilities::AudioProcessor<float> {
public:
  SynthVoice(float sampleRate, uint32_t nChannels,
             utilities::AudioBufferTools::OscillatorType oscType =
                 utilities::AudioBufferTools::OscillatorType::SINE);
  void noteOn(uint8_t pitch, uint8_t velocity);
  void noteOff(uint8_t pitch);
  void process(utilities::AudioBuffer<float> &buffer) override;
  void setFilterEnabled(bool enable);

private:
  utilities::AudioPipeline<float, WavetableOsc, dsp::AdsrEnv, dsp::BiquadFilter> _audioPipeline;
/*
  WavetableOsc _wavetableOsc;
  dsp::AdsrEnv _env{};
  dsp::BiquadFilter _filter;
*/
  float _sampleRate_Hz;
  float _freq_Hz = 440.0f;
  bool _filterEnabled = false;
};

} // namespace synthtools
#endif
