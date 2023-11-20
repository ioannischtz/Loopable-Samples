//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_AASSETDATASOURCE_H
#define OBOELIBTEST_AASSETDATASOURCE_H



#include <android/asset_manager.h>
#include "DataSource.h"

class AAssetDataSource : public DataSource {

public:
    int32_t getTotalFrames() const override { return mTotalFrames; }
    int32_t getChannelCount() const override { return mChannelCount; }
    const float* getData() const override { return mBuffer.get();	}

    static AAssetDataSource* newFromAssetManager(AAssetManager&, const char *, const int32_t);

private:

    AAssetDataSource(std::unique_ptr<float[]> data, int32_t frames,
                     const int32_t channelCount)
            : mBuffer(std::move(data))
            , mTotalFrames(frames)
            , mChannelCount(channelCount) {
    }

    const std::unique_ptr<float[]> mBuffer;
    const int32_t mTotalFrames;
    const int32_t mChannelCount;

};



#endif //OBOELIBTEST_AASSETDATASOURCE_H
