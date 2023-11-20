//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_AUDIOENGINE_H
#define OBOELIBTEST_AUDIOENGINE_H

#include <oboe/Oboe.h>
#include <android/asset_manager.h>

#include <IRestartable.h>

#include <AudioSource.h>
#include <LoopableAAssetAudio.h>

#include <LatencyTuningCallback.h>
#include <DefaultErrorCallback.h>

#include "Constants.h"

class AudioEngine : public IRestartable {
public:
    explicit AudioEngine(AAssetManager&);
    virtual ~AudioEngine() = default;



    /**
     * Open and start a stream.
     * @return error or OK
     */
    oboe::Result start();

    /**
     * Stop and close the stream.
     */
    oboe::Result stop();

    void setChannelCount(int channelCount);
    void setBufferSizeInBursts(int32_t numBursts);
    void setDeviceId(int32_t deviceId);

    /**
     * Calculate the current latency between writing a frame to the output stream and
     * the same frame being presented to the audio hardware.
     *
     * Here's how the calculation works:
     *
     * 1) Get the time a particular frame was presented to the audio hardware
     * @see AudioStream::getTimestamp
     * 2) From this extrapolate the time which the *next* audio frame written to the stream
     * will be presented
     * 3) Assume that the next audio frame is written at the current time
     * 4) currentLatency = nextFramePresentationTime - nextFrameWriteTime
     *
     * @return  Output Latency in Milliseconds
     */
    double getCurrentOutputLatencyMillis();

    bool isLatencyDetectionSupported();

    // From IRestartable
    void restart() override;
    void tap(bool isDown);
    void addAudioSource(const char *filename, int32_t loopStart, int32_t loopEnd);
    void setVolume(float vol);
    void setNoiseVolume(float vol);
    void setStart(int startSample);
    void setEnd(int endSample);
    void setNoiseStart(int startSample);
    void setNoiseEnd(int endSample);
    void toggleNoise(bool enableNoise);
    void chooseFilter(FilterType filterType);
    void setFilterCutoff(float cutFreq);

private:
    oboe::Result reopenStream();
    oboe::Result openPlaybackStream();

    AAssetManager& mAssetManager;
    std::shared_ptr<oboe::AudioStream> mStream;
    std::unique_ptr<LatencyTuningCallback> mLatencyCallback;
    std::unique_ptr<DefaultErrorCallback> mErrorCallback;

    std::shared_ptr<LoopableAAssetAudio> mAudioSource;

    bool mIsLatencyDetectionSupported = false;
    int32_t        mChannelCount = oboe::Unspecified;
    int32_t        mDeviceId = oboe::Unspecified;
    std::mutex     mLock;
};



#endif //OBOELIBTEST_AUDIOENGINE_H
