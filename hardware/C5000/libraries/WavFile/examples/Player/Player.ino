/*
 * Wave player demo:
 * Searches for the .wav files on the SD Card and opens them. 
 * Reads data from the wave files present on the SD Card and sends
 * the data to codec, which can be listened on headphone.
 *
 * This program stops after playing all the wave files present
 * in root folder of SD card
 */

#include "Audio.h"
#include "SD.h"
#include "WavFile.h"
#include "OLED.h"

WAVRead WavPlayer;

// Initializes OLED, Audio and SD modules
void setup()
{
    Serial.begin(9600);
    Serial.print("\n Audio Player Demo!\n");

    pinMode(LED0, OUTPUT);

    //Initialize OLED module for file name display	
    disp.oledInit();
    disp.clear();
    disp.setOrientation(1);
    disp.setline(0);
    disp.print("Audio Player");
    
    WavPlayer.init();
    WavPlayer.openWavFile();
    printFileInfo();

    AudioC.Audio(TRUE);
}

/*
 * This API does a simple copy of Audio samples to the Audio Out buffers of the
 * Codec, from the buffers which hold the audio data read from the wave file
 */
void processAudio()
{
  WavPlayer.copyWavBuffer(AudioC.outputLeft, AudioC.outputRight);
}

/*
 * This API reads audio data from the wave file and stores it into a temporary
 * buffer. DMA ISR copies the data from this temporary buffer to the Output
 * buffers of the Audio library
 *
 * When the API completes reading the entire data from a wave file, it tries to
 * open the next existing wave file under Root directory. If it finds the next
 * wave file, then it plays that file. When it doesn't find any more wave files,
 * it just stops reading data.
 */
void loop()
{
  int bytesRead = WavPlayer.readBuffer();
  if(bytesRead == 0)
  {
    int status = WavPlayer.openWavFile();
    AudioC.setSamplingRate(WavPlayer.samplingRate);
    printFileInfo();
    delay(500); //delay a bit to allow sample rate to stabilize
    if(WavPlayer.stopped)
    {
      disp.setline(1);
      disp.clear(1);
      disp.print("Stopped");
      while(1){}; //loop forever.
    }
  }
}

void printFileInfo()
{
  disp.clear(1);
  disp.setline(1);
  disp.print(WavPlayer.fileName);
  
  Serial.print("\nFile Name: ");
  Serial.println(WavPlayer.fileName);
  Serial.print("Number of Channels: ");
  Serial.print(WavPlayer.channelType);
  if (CHANNEL_MONO == WavPlayer.channelType)
  {
      Serial.println(" (Mono)");
  }
  else
  {
      Serial.println(" (Stereo)");
  }
  
  Serial.print("Sampling Rate: ");
  Serial.print(WavPlayer.samplingRate);
  Serial.println("Hz");

  
}
