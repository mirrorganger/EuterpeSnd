#ifndef EUTERPESND_SYNTHTOOLS_IAUDIORENDER_H
#define EUTERPESND_SYNTHTOOLS_IAUDIORENDER_H

#include <cstdint>

namespace synthtools{
/**
 * Interface to be implemented by a device capable of produce audio.
 */
    class IAudioRender {
    public:
        virtual void renderAudio(float * audioData, uint32_t numFrames) = 0;
        virtual ~IAudioRender() = default;
    };

}

#endif //EUTERPESND_SYNTHTOOLS_IAUDIORENDER_H
