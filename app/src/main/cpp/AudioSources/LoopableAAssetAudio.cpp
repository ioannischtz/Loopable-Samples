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

                switch (mFilterType) {
                    case NOFILTER: {
                        if (mEnableNoise) {
                            audioData[(i * channelCount) + j] =
                                    (mVolume - mNoiseVolume) * (sourceData[
                                                                        (mReadFrameIndex *
                                                                         channelCount) + j]) +
                                            (mNoiseVolume * dist(generator));
                        } else {
                            audioData[(i * channelCount) + j] = mVolume * (sourceData[
                                    (mReadFrameIndex * channelCount) + j]);
                        }
                        break;
                    }
                    case LOWPASS: {
                        if (mEnableNoise) {
                            audioData[(i * channelCount) + j] = (mVolume - mNoiseVolume) * (
                                    sourceData[(mReadFrameIndex * channelCount)
                                               + j])
                                    + (mNoiseVolume * lpf.filter(dist(generator))
                            );
                        } else {
                            audioData[(i * channelCount) + j] = mVolume * (sourceData[
                                    (mReadFrameIndex * channelCount) + j]);
                        }
                        break;
                    }
                    case HIGHPASS: {
                        if (mEnableNoise) {
                            audioData[(i * channelCount) + j] = (mVolume - mNoiseVolume) * (
                                    sourceData[(mReadFrameIndex * channelCount)
                                               + j])
                                    + (mNoiseVolume * hpf.filter(dist(generator))
                            );
                        } else {
                            audioData[(i * channelCount) + j] = mVolume * (sourceData[
                                    (mReadFrameIndex * channelCount) + j]);
                        }
                        break;
                    }
                }

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
                    audioData[(i * channelCount) + j] = mVolume * (sourceData[
                            (mReadFrameIndex * channelCount) + j]);
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

void LoopableAAssetAudio::renderSilence(float *start, int32_t numSamples) {
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
//    mVolume = vol;
    mVolume.store(vol);
}

float LoopableAAssetAudio::getVolume() {
    return mVolume;
}

void LoopableAAssetAudio::setNoiseVolume(float vol) {
    mNoiseVolume.store(vol);
}

void LoopableAAssetAudio::setStart(int startSample) {
    mStartPosition = startSample;
}

void LoopableAAssetAudio::setEnd(int endSample) {
    mEndPosition = endSample;
}

void LoopableAAssetAudio::setNoiseStart(int startSample) {
    mStartPositionNoise = startSample;
}

void LoopableAAssetAudio::setNoiseEnd(int endSample) {
    mEndPositionNoise = endSample;
}

void LoopableAAssetAudio::toggleNoise(bool enableNoise) {
    mEnableNoise = enableNoise;
}

void LoopableAAssetAudio::chooseFilter(FilterType filterType) {
    mFilterType = filterType;
}

void LoopableAAssetAudio::setFilterCutoff(float cutFreq) {
    mCutFreq = cutFreq;

    lpf.setup(mSampleRate, mCutFreq);
    hpf.setup(mSampleRate, mCutFreq);

}