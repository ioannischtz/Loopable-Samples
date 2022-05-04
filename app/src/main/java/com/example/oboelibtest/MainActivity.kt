package com.example.oboelibtest

import android.content.Context
import android.media.AudioDeviceInfo
import android.media.AudioManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView
import android.widget.Toast
import androidx.annotation.RequiresApi
import kotlin.properties.Delegates

class MainActivity : AppCompatActivity() {

    private lateinit var btnLoopable1: Button
    private lateinit var txtVolume: TextView
    private lateinit var seekBarVol: SeekBar

    companion object {
        private const val mChannelCount = 1
        private const val mBufferSize = 2
        private const val mFilename = "ooRAW48KHz.raw"
        private const val mStartIndex = 19285
        private const val mEndIndex = 26111
        @RequiresApi(Build.VERSION_CODES.M)
        private const val mPlaybackDevice = AudioDeviceInfo.TYPE_BUILTIN_SPEAKER
    }

    private var intVolume by Delegates.notNull<Int>()

    private var mScoStarted = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnLoopable1 = findViewById(R.id.btnLoopableSample1)
        txtVolume = findViewById(R.id.txtViewVol)
        seekBarVol =findViewById(R.id.seekBarVolume)

        txtVolume.text = seekBarVol.progress.toString()

        intVolume = 1

        val audioManager = this.getSystemService(AUDIO_SERVICE) as AudioManager
        val rate: String = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)

        setupPlaybackDevice()
        setupChannelCount()
        setupBufferSize()

        btnLoopable1.setOnTouchListener(View.OnTouchListener { v, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    Log.i("MainACtivity","TAP DOWN")
                    AudioEngine.tap(true)
                    return@OnTouchListener true
                }
                MotionEvent.ACTION_UP -> {
                    AudioEngine.tap(false)
                    return@OnTouchListener true
                }
            }
            false
        })

        seekBarVol.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            override fun onProgressChanged(seekBar: SeekBar, i: Int, b: Boolean) {
                // Display the current progress of SeekBar
                txtVolume.text = "Volume : $i"
                AudioEngine.setVolume(i.toFloat()/10)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do something
                Toast.makeText(applicationContext,"start tracking",Toast.LENGTH_SHORT).show()
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // Do something
                Toast.makeText(applicationContext,"stop tracking",Toast.LENGTH_SHORT).show()
            }
        })

        Log.i("OnCreate","OnCreate called")
    }

    override fun onResume() {
        super.onResume()

        AudioEngine.create(this)
        Log.i("OnResume","AudioEngine.create called")
        setupPlaybackDevice()
        setupChannelCount()
        setupBufferSize()

        AudioEngine.addLoopableSource(mFilename, mStartIndex, mEndIndex)
        Log.i("OnResume","AudioEngine.addLoopableSource called")

        val result = AudioEngine.start()
        Log.i("OnResume","AudioEngine.start called")



        if (result != 0) {
            showToast("Error opening stream = $result")
        } else {
            AudioEngine.setVolume(seekBarVol.progress.toFloat()/10)
        }
    }

    override fun onPause() {

        val result = AudioEngine.stop()
        if (result != 0) {
            showToast("Error stopping stream = $result")
        }
        if (mScoStarted) {
            stopBluetoothSco()
            mScoStarted = false
        }
        AudioEngine.delete()
        super.onPause()
    }


    private fun setupBufferSize() {
        AudioEngine.setBufferSizeInBursts(getBufferSizeInBursts())
    }

    private fun getBufferSizeInBursts(): Int {
        return mBufferSize
    }

    private fun setupChannelCount() {
        AudioEngine.setChannelCount(mChannelCount)
    }

    private fun setupPlaybackDevice() {
        val devices = AudioManager.GET_DEVICES_OUTPUTS

        // Start Bluetooth SCO if needed.
        if (isScoDevice(devices) && !mScoStarted) {
            startBluetoothSco()
            mScoStarted = true
        } else if (!isScoDevice(devices) && mScoStarted) {
            stopBluetoothSco()
            mScoStarted = false
        }
        AudioEngine.setAudioDeviceId(devices)
    }

    /**
     * @param deviceId
     * @return true if the device is TYPE_BLUETOOTH_SCO
     */
    @RequiresApi(api = Build.VERSION_CODES.M)
    private fun isScoDevice(deviceId: Int): Boolean {
        if (deviceId == 0) return false // Unspecified
        val audioManager = getSystemService(Context.AUDIO_SERVICE) as AudioManager
        val devices: Array<AudioDeviceInfo> =
            audioManager.getDevices(AudioManager.GET_DEVICES_OUTPUTS)
        for (device in devices) {
            if (device.id == deviceId) {
                return device.type == AudioDeviceInfo.TYPE_BLUETOOTH_SCO
            }
        }
        return false
    }

    private fun startBluetoothSco() {
        val myAudioMgr = getSystemService(Context.AUDIO_SERVICE) as AudioManager
        myAudioMgr.startBluetoothSco()
    }

    private fun stopBluetoothSco() {
        val myAudioMgr = getSystemService(Context.AUDIO_SERVICE) as AudioManager
        myAudioMgr.stopBluetoothSco()
    }

    private fun showToast(message: String?) {
        runOnUiThread {
            Toast.makeText(
                this@MainActivity,
                message,
                Toast.LENGTH_SHORT
            ).show()
        }
    }
}
