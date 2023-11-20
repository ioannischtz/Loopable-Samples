//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_AUDIOSOURCE_H
#define OBOELIBTEST_AUDIOSOURCE_H

#include <cstdint>
#include <atomic>

#include "IRenderableAudio.h"
#include "../Constants.h"


class AudioSource: public IRenderableAudio {
public:
    AudioSource(int32_t sampleRate, int32_t channelCount) :
            mSampleRate(sampleRate), mChannelCount(channelCount) {}

    virtual void setPlayingMode() = 0;
    virtual void setStopMode() = 0;
    virtual void setVolume(float vol) = 0;
    virtual float getVolume() = 0;
    virtual void setNoiseVolume(float vol) = 0;
    virtual void toggleNoise(bool enableNoise) = 0;
    virtual void setStart(int startSample) = 0;
    virtual void setEnd(int endSample) = 0;
    virtual void setNoiseStart(int startSample) = 0;
    virtual void setNoiseEnd(int endSample) = 0;
    virtual void chooseFilter(FilterType filterType) = 0;
    virtual void setFilterCutoff(float cutFreq) = 0;
//    void setPlayingMode() {
//        mIsPlaying.store(true);
//    };
//
//    void setStopMode() {
//        mIsPlaying.store(false);
//    };
//
//    virtual void setVolume(float vol) {
//        mVolume = vol;
//    }
//
//    virtual float getVolume() {
//        return mVolume;
//    }

    int32_t mSampleRate{kDefaultSampleRate};
    int32_t mChannelCount{kDefaultChannelCount};
//protected:
//    std::atomic<bool> mIsPlaying{false};
//    std::atomic<float> mVolume{0};
};

#endif //OBOELIBTEST_AUDIOSOURCE_H
