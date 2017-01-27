/*
 * FIR Filter demo:
 * Reads data from codec audio IN, sends the data to FIR filter and 
 * finally sends the filtered audio to the codec OUT which can be 
 * listened on headphone.
 *
 * Filter is configured as a Low Pass Filter(LPF) with cut-off 
 * frequency as 2kHz.
 */

#include "Audio.h"
#include "FFT.h"
#include "OLED.h"

#pragma DATA_ALIGN(32)
short fftBuf[I2S_DMA_BUF_LEN];

volatile int fftReady = 0;

void processAudio() //put your signal processing code here.
{
  copyShortBuf(AudioC.inputLeft, AudioC.outputLeft, AudioC.adcBufferSize);
  copyShortBuf(AudioC.inputRight, AudioC.outputRight, AudioC.adcBufferSize);
  if(fftReady == 0) //only process the FFT once the print routine has asynchronously finished printing out. This will miss many frames of audio, but would be good for a spectrum analyzer, to give a feel for the spectrum
  {
    copyShortBuf(AudioC.inputLeft, fftBuf, I2S_DMA_BUF_LEN);
    rfft(fftBuf,I2S_DMA_BUF_LEN,SCALE);
    fftReady = 1;
  }
  
}

void setup()
{
    int status;

    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();    
    disp.setline(1);
    
    /* Audio library is configured for non-loopback mode. */
    status = AudioC.Audio(TRUE);

    if (status == 0) //on success, report ready on OLED display.
    {
        disp.print("FFT Serial");
    }
    Serial.begin(115200);
}

void loop() //Asynchronous non-signal processing code here.
{
  if(fftReady == 1)
  {
    for(int i = 0; i < I2S_DMA_BUF_LEN; i++)
    {
      Serial.print(fftBuf[i]);
      if(i == I2S_DMA_BUF_LEN - 1)
      {
        continue;
      }
      Serial.print(", ");
    }
    Serial.print('\n');
    fftReady = 0;
  }
}
