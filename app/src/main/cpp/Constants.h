//
// Created by Ion-Hz on 5/1/2022.
//

#ifndef OBOELIBTEST_CONSTANTS_H
#define OBOELIBTEST_CONSTANTS_H

constexpr int kBufferSizeInBursts = 4; // Use 2 bursts as the buffer size (double buffer)
constexpr int32_t kBufferSizeAutomatic = 0;
constexpr int32_t kDefaultSampleRate = 48000;
constexpr int32_t kDefaultChannelCount = 1;
constexpr int32_t kOrder = 4;
constexpr int32_t kCutoff_freq_lp   = 1500;
constexpr int32_t kCutoff_freq_hp   = 100;
constexpr int32_t kCenter_freq_bs   = 7;
constexpr int32_t kWidth_freq_bs    = 10;
static const char* TAG = "AudioEngine";
enum FilterType{
    NOFILTER,LOWPASS,HIGHPASS
};


#endif //OBOELIBTEST_CONSTANTS_H
