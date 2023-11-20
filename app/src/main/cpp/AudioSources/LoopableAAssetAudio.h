//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H
#define OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H



#include <IRenderableAudio.h>
#include <memory>
#include <Constants.h>
#include "logging_macros.h"
#include <random>
#include <cmath>
#include "Iir.h"

#include "AAssetDataSource.h"

class LoopableAAssetAudio : public IRenderableAudio {
public:
    LoopableAAssetAudio(int32_t sampleRate, int32_t channelCount,
                        std::shared_ptr<DataSource> source,
                        int32_t loopStart, int32_t loopEnd) :
            mSampleRate(sampleRate), mChannelCount(channelCount),
            mAAssetSource(source),
            mStartPosition(loopStart), mEndPosition(loopEnd)
            {
                lpf.setup(mSampleRate,kCutoff_freq_lp);
                hpf.setup(mSampleRate,kCutoff_freq_hp);
            };

    ~LoopableAAssetAudio() = default;

    void renderAudio(float *audioData, int32_t numFrames) override;

    void setPlayingMode();
    void setStopMode();

    void setVolume(float vol);
    float getVolume();

    void setNoiseVolume(float vol);

    void setStart(int startSample);
    void setEnd(int endSample);
    void setNoiseStart(int startSample);
    void setNoiseEnd(int endSample);

    void toggleNoise(bool enableNoise);

    void chooseFilter(FilterType filterType);
    void setFilterCutoff(float cutFreq);

private:

    int32_t mSampleRate{kDefaultSampleRate};
    int32_t mChannelCount{kDefaultChannelCount};

    std::shared_ptr<DataSource> mAAssetSource;

    int32_t mStartPosition;
    int32_t mEndPosition;

    int32_t mStartPositionNoise;
    int32_t mEndPositionNoise;

    int32_t mReadFrameIndex = 0;

    std::atomic<bool> mIsPlaying{false};
    std::atomic<float> mVolume{0};
    std::atomic<float> mNoiseVolume{0};
    bool mIsLooping{false};
    bool mEnableNoise{false};

    FilterType mFilterType{NOFILTER};
    float mCutFreq{100};

    std::default_random_engine generator;
    std::normal_distribution<float> dist{0.0f,0.1f};

    Iir::Butterworth::LowPass<kOrder> lpf;
    Iir::Butterworth::HighPass<kOrder> hpf;

    static void renderSilence(float*, int32_t);
};


#endif //OBOELIBTEST_LOOPABLEAASSETDATASOURCE_H
