//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H
#define OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H



#include <IRenderableAudio.h>
#include <memory>
#include <Constants.h>
#include "logging_macros.h"

#include "AAssetDataSource.h"

class LoopableAAssetAudio : public IRenderableAudio {
public:
    LoopableAAssetAudio(int32_t sampleRate, int32_t channelCount,
                        std::shared_ptr<DataSource> source,
                        int32_t loopStart, int32_t loopEnd) :
            mSampleRate(sampleRate), mChannelCount(channelCount),
            mAAssetSource(source),
            mStartPosition(loopStart), mEndPosition(loopEnd) { LOGI("LoopableAAssetAudio Constructor"); };

    ~LoopableAAssetAudio() = default;

    void renderAudio(float *audioData, int32_t numFrames) override;

    void setPlayingMode();

    void setStopMode();

    void setVolume(float vol) ;

    float getVolume();

private:

    int32_t mSampleRate{kDefaultSampleRate};
    int32_t mChannelCount{kDefaultChannelCount};

    std::shared_ptr<DataSource> mAAssetSource;

    int32_t mStartPosition;
    int32_t mEndPosition;

    int32_t mReadFrameIndex = 0;

    std::atomic<bool> mIsPlaying{false};
    std::atomic<float> mVolume{0};
    bool mIsLooping{false};

    static void renderSilence(float*, int32_t);
};


#endif //OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H
