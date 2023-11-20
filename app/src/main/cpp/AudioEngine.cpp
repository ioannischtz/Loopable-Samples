//
// Created by Ion-Hz on 5/1/2022.
//

#include "AudioEngine.h"

AudioEngine::AudioEngine(AAssetManager &assetManager)
        : mAssetManager(assetManager),
          mLatencyCallback(std::make_unique<LatencyTuningCallback>()),
          mErrorCallback(std::make_unique<DefaultErrorCallback>(*this)) {
}

//-----------------------------AudioEngine Lifecycle---------------------------------------------//

oboe::Result AudioEngine::start() {
    std::lock_guard<std::mutex> lock(mLock);
    mIsLatencyDetectionSupported = false;
    auto result = openPlaybackStream();
    if (result == oboe::Result::OK) {
//        mLatencyCallback->setSource(std::dynamic_pointer_cast<IRenderableAudio>(mAudioSource));
        mLatencyCallback->setSource(std::dynamic_pointer_cast<IRenderableAudio>(mAudioSource));

        LOGD("Stream opened: AudioAPI = %d, channelCount = %d, deviceID = %d",
             mStream->getAudioApi(),
             mStream->getChannelCount(),
             mStream->getDeviceId());

        result = mStream->start();
        if (result != oboe::Result::OK) {
            LOGE("Error starting playback stream. Error: %s", oboe::convertToText(result));
            mStream->close();
            mStream.reset();
        } else {
            mIsLatencyDetectionSupported = (mStream->getTimestamp((CLOCK_MONOTONIC)) !=
                                            oboe::Result::ErrorUnimplemented);
        }
    } else {
        LOGE("Error creating playback stream. Error: %s", oboe::convertToText(result));
    }
    return result;
}

oboe::Result AudioEngine::stop() {
    oboe::Result result = oboe::Result::OK;
    // Stop, close and delete in case not already closed.
    std::lock_guard<std::mutex> lock(mLock);
    if (mStream) {
        result = mStream->stop();
        mStream->close();
        mStream.reset();
    }
    return result;
}

void AudioEngine::setChannelCount(int channelCount) {
    if (mChannelCount != channelCount) {
        mChannelCount = channelCount;
        reopenStream();
    }
}

void AudioEngine::setBufferSizeInBursts(int32_t numBursts) {
    std::lock_guard<std::mutex> lock(mLock);
    if (!mStream) return;

    mLatencyCallback->setBufferTuneEnabled(numBursts == kBufferSizeAutomatic);
    auto result = mStream->setBufferSizeInFrames(
            numBursts * mStream->getFramesPerBurst());
    if (result) {
        LOGD("Buffer size successfully changed to %d", result.value());
    } else {
        LOGW("Buffer size could not be changed, %d", result.error());
    }
}

void AudioEngine::setDeviceId(int32_t deviceId) {
    if (mDeviceId != deviceId) {
        mDeviceId = deviceId;
        if (reopenStream() != oboe::Result::OK) {
            LOGW("Open stream failed, forcing deviceId to Unspecified");
            mDeviceId = oboe::Unspecified;
        }
    }
}

//----------------------------AudioEngine Latency Tuning ----------------------------------------//

double AudioEngine::getCurrentOutputLatencyMillis() {
    if (!mIsLatencyDetectionSupported) return -1.0;

    std::lock_guard<std::mutex> lock(mLock);
    if (!mStream) return -1.0;

    oboe::ResultWithValue<double> latencyResult = mStream->calculateLatencyMillis();
    if (latencyResult) {
        return latencyResult.value();
    } else {
        LOGE("Error calculating latency: %s", oboe::convertToText(latencyResult.error()));
        return -1.0;
    }
}

bool AudioEngine::isLatencyDetectionSupported() {
    return mIsLatencyDetectionSupported;
}

//----------------------------AudioEngine Interaction ----------------------------------------//

void AudioEngine::restart() {
    // The stream will have already been closed by the error callback.
    mLatencyCallback->reset();
    start();
}

void AudioEngine::tap(bool isDown ) {
    std::lock_guard<std::mutex> lock(mLock);
    isDown ? mAudioSource->setPlayingMode()
           : mAudioSource->setStopMode();
}

void AudioEngine::toggleNoise(bool enableNoise) {
    mAudioSource->toggleNoise(enableNoise);
}

void AudioEngine::addAudioSource(const char *filename, int32_t loopStart, int32_t loopEnd){
    std::lock_guard<std::mutex> lock(mLock);
    LOGI("addAudioSource called");

    if (mChannelCount == oboe::Unspecified) {
        LOGE("mChannelCount == oboe::Unspecified");
    }
    std::shared_ptr<AAssetDataSource> mAssetSource {
        AAssetDataSource::newFromAssetManager(mAssetManager, filename, mChannelCount)
    };
    if (mAssetSource == nullptr){
        LOGE("Could not load source data for loopable source sound");
//        return oboe::result::error
    }
    LOGI("call std::make_shared<LoopableAAssetAudio>");
    LOGI("mStream = %d",mStream != nullptr);
    mAudioSource = std::make_shared<LoopableAAssetAudio>(
            kDefaultSampleRate, mChannelCount,
            mAssetSource,
            loopStart,loopEnd
            );
//    LOGI("call std::make_shared<LoopableAAssetAudio>");
//    mAudioSource =
//            static_cast<std::shared_ptr<LoopableAAssetAudio>>(new LoopableAAssetAudio(
//                    mStream->getSampleRate(), mChannelCount,
//                    mAssetSource,
//                    loopStart, loopEnd
//            ));


    LOGI("std::make_shared<LoopableAAssetAudio>");
}

void AudioEngine::setVolume(float vol) {
    mAudioSource->setVolume(vol);
}

void AudioEngine::setNoiseVolume(float vol) {
    mAudioSource->setNoiseVolume(vol);
}

void AudioEngine::setStart(int startSample) {
    mAudioSource->setStart(startSample);
}
void AudioEngine::setEnd(int endSample) {
    mAudioSource->setEnd(endSample);
}
void AudioEngine::setNoiseStart(int startSample) {
    mAudioSource->setNoiseStart(startSample);
}
void AudioEngine::setNoiseEnd(int endSample) {
    mAudioSource->setNoiseStart(endSample);
}

void AudioEngine::chooseFilter(FilterType filterType) {
    mAudioSource->chooseFilter(filterType);
}

void AudioEngine::setFilterCutoff(float cutFreq) {
    mAudioSource->setFilterCutoff(cutFreq);
}

//----------------------------AudioEngine private methods ---------------------------------------//

oboe::Result AudioEngine::reopenStream() {
    if (mStream) {
        stop();
        return start();
    } else {
        return oboe::Result::OK;
    }
}

oboe::Result AudioEngine::openPlaybackStream() {
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setFormat(oboe::AudioFormat::Float)
            ->setFormatConversionAllowed(true)
            ->setDataCallback(mLatencyCallback.get())
            ->setErrorCallback(mErrorCallback.get())
            ->setChannelCount(mChannelCount)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
            ->openStream(mStream);
    if (result == oboe::Result::OK) {
        mChannelCount = mStream->getChannelCount();
    }
    return result;
}












