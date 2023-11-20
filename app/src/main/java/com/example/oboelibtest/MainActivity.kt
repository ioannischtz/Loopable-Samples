package com.example.oboelibtest

import android.app.Activity
import android.content.Context
import android.media.AudioDeviceInfo
import android.media.AudioManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.view.inputmethod.EditorInfo
import android.widget.*
import androidx.annotation.RequiresApi
import kotlin.properties.Delegates

class MainActivity : AppCompatActivity() {

    private lateinit var btnLoopable1: Button
    private lateinit var txtVolume: TextView
    private lateinit var seekBarVol: SeekBar
    private lateinit var txtLoopStart: EditText
    private lateinit var txtLoopEnd: EditText
    private lateinit var chkToggleNoise: CheckBox
    private lateinit var txtNoiseStart: EditText
    private lateinit var txtNoiseEnd: EditText
    private lateinit var txtCutoff: EditText
    private lateinit var seekBarNoiseVol: SeekBar


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
        seekBarVol = findViewById(R.id.seekBarVolume)
        txtLoopStart = findViewById(R.id.editNumLoopStart)
        txtLoopEnd = findViewById(R.id.LoopEnd)
        chkToggleNoise = findViewById(R.id.chkEnableNoise)
        txtNoiseStart = findViewById(R.id.editNumNoiseStart)
        txtNoiseEnd = findViewById(R.id.editNumNoiseEnd)
        txtCutoff = findViewById(R.id.editNumCutoffF)
        seekBarNoiseVol = findViewById(R.id.seekBarNoiseVol)

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
                    Log.i("MainACtivity", "TAP DOWN")
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

        txtLoopStart.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_UNSPECIFIED || actionId == EditorInfo.IME_ACTION_GO) {
                AudioEngine.setStart(txtLoopStart.text.toString().toInt())
                hideKeyboard()
                true
            } else {
                false
            }
        }
        txtLoopEnd.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_UNSPECIFIED || actionId == EditorInfo.IME_ACTION_GO) {
                AudioEngine.setEnd(txtLoopEnd.text.toString().toInt())
                hideKeyboard()
                true
            } else {
                false
            }
        }
        txtNoiseStart.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_UNSPECIFIED || actionId == EditorInfo.IME_ACTION_GO) {
                AudioEngine.setNoiseStart(txtNoiseStart.text.toString().toInt())
                hideKeyboard()
                true
            } else {
                false
            }
        }
        txtNoiseEnd.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_UNSPECIFIED || actionId == EditorInfo.IME_ACTION_GO) {
                AudioEngine.setNoiseEnd(txtNoiseEnd.text.toString().toInt())
                hideKeyboard()
                true
            } else {
                false
            }
        }

        txtCutoff.setOnEditorActionListener { v, actionId, event ->
            if (actionId == EditorInfo.IME_ACTION_DONE || actionId == EditorInfo.IME_ACTION_UNSPECIFIED || actionId == EditorInfo.IME_ACTION_GO) {
                AudioEngine.setFilterCutoff(txtCutoff.text.toString().toFloat())
                hideKeyboard()
                true
            } else {
                false
            }
        }

        chkToggleNoise.setOnCheckedChangeListener { buttonView, isChecked ->
            AudioEngine.toggleNoise(isChecked)
        }

        seekBarVol.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            override fun onProgressChanged(seekBar: SeekBar, i: Int, b: Boolean) {
                // Display the current progress of SeekBar
                txtVolume.text = "Volume : $i"
                AudioEngine.setVolume(i.toFloat() / 10)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do something
//                Toast.makeText(applicationContext,"start tracking",Toast.LENGTH_SHORT).show()
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // Do something
//                Toast.makeText(applicationContext,"stop tracking",Toast.LENGTH_SHORT).show()
            }
        })

        seekBarNoiseVol.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            override fun onProgressChanged(seekBar: SeekBar, i: Int, b: Boolean) {
                // Display the current progress of SeekBar
                AudioEngine.setNoiseVolume(i.toFloat() / 10)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {
                // Do something
//                Toast.makeText(applicationContext,"start tracking",Toast.LENGTH_SHORT).show()
            }

            override fun onStopTrackingTouch(seekBar: SeekBar) {
                // Do something
//                Toast.makeText(applicationContext,"stop tracking",Toast.LENGTH_SHORT).show()
            }
        })

    }

    override fun onResume() {
        super.onResume()

        AudioEngine.create(this)
        Log.i("OnResume", "AudioEngine.create called")
        setupPlaybackDevice()
        setupChannelCount()
        setupBufferSize()

        AudioEngine.addLoopableSource(mFilename, mStartIndex, mEndIndex)
        Log.i("OnResume", "AudioEngine.addLoopableSource called")

        val result = AudioEngine.start()
        Log.i("OnResume", "AudioEngine.start called")



        if (result != 0) {
            showToast("Error opening stream = $result")
        } else {
            AudioEngine.setVolume(seekBarVol.progress.toFloat() / 10)
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

    fun onRadioButtonClicked(view: View) {
        if (view is RadioButton) {
            // Is the button now checked?
            val checked = view.isChecked

            // Check which radio button was clicked
            when (view.getId()) {
                R.id.radioNoFilter ->
                    if (checked) {
                        AudioEngine.chooseFilter(AudioEngine.FilterType.NOFILTER)
                    }
                R.id.radioLowPass ->
                    if (checked) {
                        AudioEngine.chooseFilter(AudioEngine.FilterType.LOWPASS)
                    }
                R.id.radioHighPass ->
                    if (checked) {
                        AudioEngine.chooseFilter(AudioEngine.FilterType.HIGHPASS)
                    }
            }
        }

    }

}
