#include "AudioDevice.h"
#include "Conversions.h"
#include "SynthVoice.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main() {

  audioDevice::DeviceParameters params;
  bool filterEnabled = false;
  params.framesPerBuffer = 256U;
  params.nChannels = 2U;
  params.sampleRateHz = 44'100U;
  auto outputDevice =
      audioDevice::AudioDeviceBuilder::createOutputDevice(params);

  synthtools::SynthVoice voice(
      static_cast<float>(params.sampleRateHz), params.nChannels,
      utilities::AudioBufferTools::OscillatorType::SINE);
  outputDevice->setAudioProcessor(voice);

  if (outputDevice->open()) {
    uint8_t initialPitch = 40U;
    voice.noteOn(40U, 100U);
    char input;
    do {
      std::cin >> input;
      if (input == '+') {
        ++initialPitch;
        std::cout << "Increasing " << int(initialPitch) << ", "
                  << utilities::Conversions::pitchToFrequency(initialPitch)
                  << " Hz"
                  << "\n";
      } else if (input == '-') {
        --initialPitch;
        std::cout << "Decreasing " << int(initialPitch) << ", "
                  << utilities::Conversions::pitchToFrequency(initialPitch)
                  << " Hz"
                  << "\n";
      } else if (input == 'p') {
        std::cout << "Playing " << int(initialPitch) << ", "
                  << utilities::Conversions::pitchToFrequency(initialPitch)
                  << " Hz"
                  << "\n";
        voice.noteOn(--initialPitch, 100U);
      } else if (input == 's') {
        std::cout << "Stopping \n";
        voice.noteOff(initialPitch);
      } else if (input == 'f') {
        filterEnabled = !filterEnabled;
        std::cout << "Filter enabled " << filterEnabled << "\n";
        voice.setFilterEnabled(filterEnabled);
      }
    } while (input != 'q');

    voice.noteOff(60U);
  }

  outputDevice->stop();

  return 0;
}