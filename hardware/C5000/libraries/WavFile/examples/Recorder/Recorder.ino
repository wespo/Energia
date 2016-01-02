/*
 * Wave recorder demo:
 *  Stores the audio samples received from audio module to a file
 *  on SD card in wave format.
 *
 * Default configurations:
 *  Sampling rate - 48KHz
 *  Channel Type  - Mono
 *  Duration      - 60 secs
 *
 * Note: Stereo mode configurations can be enabled by defining macro
 * 'ENABLE_STEREO_RECORD'
 *
 * Changing the default configurations:
 *  Sampling rate - Change the value of 'samplingRate' which
 *                  can be one of the macros SAMPLING_RATE_XX_KHZ
 *                  defined in Audio.h
 *  Channel Type  - Change the value of 'channelType' to CHANNEL_MONO
 *                  for mono recording
 *  Duration      - Change the value of macro 'RECORD_DURATION' which
 *                  defines the duration of record in secs
 *
 * CAUTION: DO NOT power off the DSP shield when record is in progress.
 * Recorded file will not be saved unless program ends smoothly.
 *
 * SD Card performance Requirements for proper record
 * ---------------------------------------------------
 * SD card used for record should be of class 4 or higher for proper 
 * recording in stereo mode. Recording in stereo mode can be done 
 * without data loss up to 32KHz sampling frequency with class 4 SD card.
 * It is not recommended to use sampling frequency more than 32KHz 
 * for stereo mode which causes data loss during record.
 *
 */

#include "SD.h"
#include "Audio.h"
#include "OLED.h"
#include "WavFile.h"

WAVWrite WavRecorder;

void processAudio()
{
  WavRecorder.captureAudio(AudioC.inputLeft, AudioC.inputRight, AudioC.adcBufferSize); //For Stereo
  //WavRecorder.captureAudio(AudioC.inputLeft, AudioC.adcBufferSize); //For Mono
}

void setup()
{
    pinMode(LED0, OUTPUT);

    //Initialize OLED module for file name display
    disp.oledInit();
    disp.clear();
    disp.setOrientation(1);
    disp.setline(0);
    disp.print("Wave Recorder");
    disp.setline(1);
    disp.print("Running");
    
    Serial.begin(9600);
    
    
    WavRecorder.begin(SAMPLING_RATE_32_KHZ, CHANNEL_STEREO, 10); //For Stereo
    //WavRecorder.begin(SAMPLING_RATE_48_KHZ, CHANNEL_MONO, 10); //For Mono
    AudioC.setInputGain(60, 60);
}


void loop()
{
    if(!WavRecorder.storeAudio())
    {
        digitalWrite(LED0, 0);
        disp.setline(1);
        disp.clear(1);
        disp.print("Stopped");
        while(1){};
    }
}
