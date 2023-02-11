#include "AudioDevice.h"
#include "RtAudioDevice.h"
namespace audioDevice
{


std::unique_ptr<OutputDevice<float>> AudioDeviceBuilder::createOutputDevice(const DeviceParameters& params){
   auto rtAudioDevice = std::make_unique<RtAudioDevice>(params);
   return std::move(rtAudioDevice); 
}



}

