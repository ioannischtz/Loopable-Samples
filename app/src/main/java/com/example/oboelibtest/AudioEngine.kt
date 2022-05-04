package com.example.oboelibtest

import android.content.Context
import android.content.res.AssetManager
import android.media.AudioManager

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



object AudioEngine {
    var mEngineHandle: Long = 0
    fun create(context: Context): Boolean {
        if (mEngineHandle == 0L) {
            setDefaultStreamValues(context)
            mEngineHandle = native_createEngine(context.assets)
        }
        return mEngineHandle != 0L
    }

    private fun setDefaultStreamValues(context: Context) {
        val myAudioMgr = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        val sampleRateStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)
        val defaultSampleRate = sampleRateStr.toInt()
        val framesPerBurstStr =
            myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)
        val defaultFramesPerBurst = framesPerBurstStr.toInt()
        native_setDefaultStreamValues(defaultSampleRate, defaultFramesPerBurst)
    }

    fun start(): Int {
        return if (mEngineHandle != 0L) {
            native_startEngine(mEngineHandle)
        } else {
            -1
        }
    }

    fun stop(): Int {
        return if (mEngineHandle != 0L) {
            native_stopEngine(mEngineHandle)
        } else {
            -1
        }
    }

    fun delete() {
        if (mEngineHandle != 0L) {
            native_deleteEngine(mEngineHandle)
        }
        mEngineHandle = 0
    }

    fun addLoopableSource(filename: String, start: Int, end: Int) {
        if (mEngineHandle != 0L) native_addLoopableSource(mEngineHandle, filename, start, end)
    }

    fun tap(isDown: Boolean) {
        if (mEngineHandle != 0L) native_tap(mEngineHandle, isDown)
    }

    fun setVolume(amplitude: Float) {
        if (mEngineHandle != 0L) native_setVolume(mEngineHandle, amplitude)
    }

    fun setAudioDeviceId(deviceId: Int) {
        if (mEngineHandle != 0L) native_setAudioDeviceId(mEngineHandle, deviceId)
    }

    fun setChannelCount(channelCount: Int) {
        if (mEngineHandle != 0L) native_setChannelCount(mEngineHandle, channelCount)
    }

    fun setBufferSizeInBursts(bufferSizeInBursts: Int) {
        if (mEngineHandle != 0L) native_setBufferSizeInBursts(mEngineHandle, bufferSizeInBursts)
    }

    // Native methods
    private external fun native_createEngine(assetManager: AssetManager): Long
    private external fun native_startEngine(engineHandle: Long): Int
    private external fun native_stopEngine(engineHandle: Long): Int
    private external fun native_deleteEngine(engineHandle: Long)

    private external fun native_addLoopableSource(mEngineHandle: Long, filename: String, start: Int, end: Int)
    private external fun native_tap(engineHandle: Long, isDown: Boolean)
    private external fun native_setVolume(engineHandle: Long, volume: Float)

    private external fun native_setAudioDeviceId(engineHandle: Long, deviceId: Int)
    private external fun native_setChannelCount(mEngineHandle: Long, channelCount: Int)
    private external fun native_setBufferSizeInBursts(engineHandle: Long, bufferSizeInBursts: Int)
    private external fun native_setDefaultStreamValues(sampleRate: Int, framesPerBurst: Int)

    // Load native library
    init {
        System.loadLibrary("audioengine")
    }
}