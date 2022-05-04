//
// Created by Ion-Hz on 4/10/2022.
//

#include "LoopableAAssetAudio.h"

void LoopableAAssetAudio::renderAudio(float *audioData, int32_t numFrames) {
    const int32_t channelCount = mAAssetSource->getChannelCount();
//    const int32_t sampleRate = mWavSource->getSampleRate();

    if (mIsPlaying) {
        int32_t framesToRender = numFrames;
        int32_t totalSourceFrames = mAAssetSource->getTotalFrames();
        const float *sourceData = mAAssetSource->getData();

        if (!mIsLooping && mReadFrameIndex + numFrames >= totalSourceFrames) {
            framesToRender = totalSourceFrames - mReadFrameIndex;
            mIsPlaying = false;
        }

        for (int i = 0; i < framesToRender; ++i) {
            for (int j = 0; j < channelCount; ++j) {
                audioData[(i * channelCount) + j] = mVolume * sourceData[
                                            (mReadFrameIndex * channelCount) + j];
            }

            if (mReadFrameIndex == mStartPosition) {
                mIsLooping = true;
            }

            // Increment and handle wraparound
            if (++mReadFrameIndex >= mEndPosition) {
                mReadFrameIndex = mStartPosition;
            }

        }

        if (framesToRender < numFrames) {
            // fill the rest of the buffer with silence
            renderSilence(&audioData[framesToRender], numFrames * channelCount);
        }


    } else {
        if (mReadFrameIndex > 0) {
            int32_t totalSourceFrames = mAAssetSource->getTotalFrames();
            int32_t framesToRender = numFrames;
            const float *sourceData = mAAssetSource->getData();

            if (mReadFrameIndex + numFrames >= totalSourceFrames) {
                framesToRender = totalSourceFrames - mReadFrameIndex;
            }

            for (int i = 0; i < framesToRender; ++i) {
                for (int j = 0; j < channelCount; ++j) {
                    audioData[(i * channelCount) + j] = mVolume * sourceData[
                            (mReadFrameIndex * channelCount) + j];
                }
                ++mReadFrameIndex;
            }

            if (framesToRender < numFrames) {
                // fill the rest of the buffer with silence
                renderSilence(&audioData[framesToRender], numFrames * channelCount);
                mReadFrameIndex = 0;
            }
        } else {
            renderSilence(&audioData[numFrames], numFrames * channelCount);
            mReadFrameIndex = 0;
        }
    }
}

void LoopableAAssetAudio::renderSilence(float *start, int32_t numSamples){
    for (int i = 0; i < numSamples; ++i) {
        start[i] = 0;
    }
}

void LoopableAAssetAudio::setPlayingMode() {
    mIsPlaying.store(true);
}

void LoopableAAssetAudio::setStopMode() {
    mIsPlaying.store(false);
}

void LoopableAAssetAudio::setVolume(float vol) {
    mVolume = vol;
}

float LoopableAAssetAudio::getVolume() {
    return mVolume;
}
