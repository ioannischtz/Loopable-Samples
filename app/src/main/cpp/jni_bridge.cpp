//
// Created by Ion-Hz on 3/30/2022.
//

/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <oboe/Oboe.h>
#include <android/asset_manager_jni.h>

#include "logging_macros.h"
#include "AudioEngine.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Creates the audio engine
 *
 * @return a pointer to the audio engine. This should be passed to other methods
 */
JNIEXPORT jlong JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1createEngine(JNIEnv *env, jobject thiz,
                                                              jobject asset_manager) {
    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    if (assetManager == nullptr) {
        LOGE("Could not obtain the AAssetManager");
        return 0;
    }

    // We use std::nothrow so `new` returns a nullptr if the engine creation fails
    AudioEngine *engine = new(std::nothrow) AudioEngine(*assetManager);
    if (engine == nullptr) {
        LOGE("Could not instantiate AudioEngine");
        return 0;
    }
//    engine->addAudioSource("ooRAW48KHz.raw",16000,24000);
    return reinterpret_cast<jlong>(engine);
}

JNIEXPORT jint JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1startEngine(JNIEnv *env, jobject thiz,
                                                             jlong engine_handle) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(engine_handle);
    return static_cast<jint>(engine->start());
}

JNIEXPORT jint JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1stopEngine(JNIEnv *env, jobject thiz,
                                                            jlong engine_handle) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(engine_handle);
    return static_cast<jint>(engine->stop());
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1deleteEngine(JNIEnv *env, jobject thiz,
                                                              jlong engine_handle) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(engine_handle);
    engine->stop();
    delete engine;
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1addLoopableSource(JNIEnv *env, jobject thiz,
                                                                   jlong m_engine_handle,
                                                                   jstring filename, jint start,
                                                                   jint end) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(m_engine_handle);
    const char* filenameNative = env->GetStringUTFChars(filename, nullptr);
//    engine->addAudioSource(reinterpret_cast<const char *>(filename), start, end);
    engine->addAudioSource(filenameNative, start, end);

}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1tap(JNIEnv *env, jobject thiz, jlong engine_handle,
                                                     jboolean is_down) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(engine_handle);
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->tap(is_down);
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1setVolume(JNIEnv *env, jobject thiz,
                                                           jlong engine_handle, jfloat volume) {
    AudioEngine *engine = reinterpret_cast<AudioEngine *>(engine_handle);
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setVolume(volume);
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1setAudioDeviceId(JNIEnv *env, jobject thiz,
                                                                  jlong engine_handle,
                                                                  jint device_id) {
    AudioEngine *engine = reinterpret_cast<AudioEngine*>(engine_handle);
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setDeviceId(device_id);
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1setChannelCount(JNIEnv *env, jobject thiz,
                                                                 jlong m_engine_handle,
                                                                 jint channel_count) {
    AudioEngine *engine = reinterpret_cast<AudioEngine*>(m_engine_handle);
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setChannelCount(channel_count);
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1setBufferSizeInBursts(JNIEnv *env, jobject thiz,
                                                                       jlong engine_handle,
                                                                       jint buffer_size_in_bursts) {
    AudioEngine *engine = reinterpret_cast<AudioEngine*>(engine_handle);
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setBufferSizeInBursts(buffer_size_in_bursts);
}

JNIEXPORT void JNICALL
Java_com_example_oboelibtest_AudioEngine_native_1setDefaultStreamValues(JNIEnv *env, jobject thiz,
                                                                        jint sample_rate,
                                                                        jint frames_per_burst) {
    oboe::DefaultStreamValues::SampleRate = (int32_t) sample_rate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) frames_per_burst;
}

#ifdef __cplusplus
}
#endif




