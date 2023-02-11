#include "AudioDevice.h"
#include "Conversions.h"
#include "SynthVoice.h"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

int main(){

    audioDevice::DeviceParameters params;
    params.framesPerBuffer = 256U;
    params.nChannels = 2U;
    params.sampleRateHz = 44'100U;
    auto outputDevice = audioDevice::AudioDeviceBuilder::createOutputDevice(params);

    synthtools::SynthVoice voice(static_cast<float>(params.sampleRateHz),utilities::AudioBufferTools::OscillatorType::SQUARE); 
    outputDevice->setAudioProcessor(voice);

    if(outputDevice->open()){
        uint8_t initialPitch = 40U;
        voice.noteOn(40U,100U);
        char input;
        do{
            std::cout<< std::endl;
            std::cin  >> input;
            if(input == '+'){
                ++initialPitch;
                std::cout << "Increasing " << int(initialPitch)<< ", "<<  utilities::Conversions::pitchToFrequency(initialPitch) << " Hz"<<"\n";
            }
            else if(input == '-'){
                --initialPitch;
                std::cout << "Decreasing " << int(initialPitch)<< ", "<<  utilities::Conversions::pitchToFrequency(initialPitch) << " Hz"<<"\n";
            }
            else if(input == 'p'){
                std::cout << "Playing " << int(initialPitch)<< ", "<<  utilities::Conversions::pitchToFrequency(initialPitch) << " Hz"<<"\n";
                voice.noteOn(--initialPitch,100U);
            }
            else if(input == 's'){
                std::cout << "Stopping \n";
                voice.noteOff(initialPitch);
            }

        }   
        while(input != 'q');
        
        voice.noteOff(60U);
    }

    outputDevice->stop();


    return 0;
}