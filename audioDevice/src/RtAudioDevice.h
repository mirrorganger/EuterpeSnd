#ifndef EUTERPESND_AUDIO_DEVICE_RTAUDIO_DEVICE
#define EUTERPESND_AUDIO_DEVICE_RTAUDIO_DEVICE

#include <RtAudio.h>
#include "AudioDevice.h"

namespace audioDevice
{

class RtAudioDevice : public OutputDevice<float>{
public:
    RtAudioDevice(const DeviceParameters deviceParameters);
    ~RtAudioDevice();
    
    bool open() override;
    void stop() override;
    void setAudioProcessor(utilities::AudioProcessor<float>& processor) override;
    
    static int RtAudioCallbackHandler(void *outputBuffer, void *inputBuffer,
                                      unsigned int nFrames,
                                      double streamTime,
                                      RtAudioStreamStatus status,
                                     void *userData ){
      RtAudioDevice* _this = (RtAudioDevice*)(userData);
      float * tempOutputBuffer = (float*)outputBuffer;
      float * tempInputBuffer = (float*)outputBuffer;                                 
      _this->audioCallback(tempOutputBuffer,tempInputBuffer,nFrames);
      return 0;
    }


private:
    void audioCallback(float* outputBuffer, float* inputBuffer, uint32_t numberOfFrames);
    DeviceParameters _deviceParameters;
    RtAudio _dacInterface;
    utilities::AudioProcessor<float>* _audioProcessor;
};

}


#endif



