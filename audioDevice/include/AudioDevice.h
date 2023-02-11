#ifndef EUTERPESND_AUDIO_DEVICE_AUDIO_DEVICE_H
#define EUTERPESND_AUDIO_DEVICE_AUDIO_DEVICE_H

#include "AudioBuffer.h"
#include <memory>

namespace audioDevice
{

  struct DeviceParameters{
    uint32_t sampleRateHz;
    uint32_t nChannels;
    uint32_t framesPerBuffer;
  };

  template<typename DataType>  
  class OutputDevice{
    public:
    virtual bool open()=0;
    virtual void stop()=0;
    virtual void setAudioProcessor(utilities::AudioProcessor<DataType>& processor) =0;
    virtual ~OutputDevice() = default;
  };

  class AudioDeviceBuilder
  {
  public:
    static std::unique_ptr<OutputDevice<float>> createOutputDevice(const DeviceParameters& params);
  };

} 




#endif