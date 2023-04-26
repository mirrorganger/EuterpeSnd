
#include "RtAudioDevice.h"

namespace audioDevice {

RtAudioDevice::RtAudioDevice(const DeviceParameters deviceParameters)
    : _deviceParameters(deviceParameters) {}

RtAudioDevice::~RtAudioDevice() {
  if (_dacInterface.isStreamOpen())
    _dacInterface.closeStream();
}

bool RtAudioDevice::open() {

  if (_dacInterface.getDeviceCount() < 1) {
    std::cerr << "\n No Audio Devices Found! \n";
    return false;
  }

  RtAudio::StreamParameters streamParameters;

  streamParameters.deviceId = _dacInterface.getDefaultOutputDevice();
  streamParameters.nChannels = _deviceParameters.nChannels;
  streamParameters.firstChannel = 0U;

  try {
    _dacInterface.openStream(
        &streamParameters, nullptr, RTAUDIO_FLOAT32,
        _deviceParameters.sampleRateHz, &_deviceParameters.framesPerBuffer,
        &RtAudioDevice::RtAudioCallbackHandler, (void *)(this));
    _dacInterface.startStream();
  } catch (RtAudioErrorType &e) {
    std::cerr << "Error " << e << "\n";
    return false;
  }

  return true;
}

void RtAudioDevice::stop() {
  try {
    _dacInterface.stopStream();
  } catch (RtAudioErrorType &e) {
    std::cout << "Error " << e << "\n";
  }
}

void RtAudioDevice::setAudioProcessor(
    utilities::AudioProcessor<float> &processor) {
  _audioProcessor = &processor;
}

void RtAudioDevice::audioCallback(float *outputBuffer, float *inputBuffer,
                                  uint32_t numberOfFrames) {
  // TODO : way of checking the parameters are actually what we set previously
  utilities::AudioBuffer<float> audioBuffer(outputBuffer,
                                            _deviceParameters.nChannels,
                                            _deviceParameters.framesPerBuffer);
  if (_audioProcessor != nullptr) {
    _audioProcessor->process(audioBuffer);
  }
}

} // namespace audioDevice
