//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_IRENDERABLEAUDIO_H
#define OBOELIBTEST_IRENDERABLEAUDIO_H

#include <cstdint>

class IRenderableAudio {

public:
    virtual ~IRenderableAudio() = default;
    virtual void renderAudio(float *audioData, int32_t numFrames) = 0;
};

#endif //OBOELIBTEST_IRENDERABLEAUDIO_H
