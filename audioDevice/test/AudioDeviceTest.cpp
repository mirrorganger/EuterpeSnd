#include "AudioDevice.h"

#include <AudioBufferTools.h>

#include <gmock/gmock.h>

using namespace audioDevice;
using namespace testing;

class AudioDeviceTest : public Test {

protected:
  audioDevice::OutputDevice<float> *_outputDevice;
};
