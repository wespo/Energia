/*
 * Spectrum Analyzer Demo for OLED:
 * Reads data from codec audio IN, sends the data to FFT and power  
 * spectral density. Passes signal back out and displays spectrum
 * on OLED display
 *
 */

#include "Audio.h"
#include "FFT.h"
#include "OLED.h"

#pragma DATA_ALIGN(32)
short fftBuf[I2S_DMA_BUF_LEN];
#define WINDOW_LENGTH 512
int window[WINDOW_LENGTH] = {0,0,2,4,7,11,16,22,29,36,45,54,64,76,88,101,115,130,146,163,181,200,221,242,264,287,311,336,363,390,419,448,479,511,545,579,615,651,690,729,770,811,855,899,945,993,1041,1091,1143,1196,1250,1306,1364,1423,1483,1545,1609,1674,1741,1810,1880,1952,2025,2100,2177,2256,2336,2419,2503,2589,2676,2766,2857,2950,3046,3143,3242,3342,3445,3550,3657,3766,3876,3989,4104,4220,4339,4460,4583,4708,4834,4963,5094,5227,5362,5500,5639,5780,5924,6069,6216,6366,6517,6671,6827,6984,7144,7306,7469,7635,7803,7972,8144,8318,8493,8670,8850,9031,9214,9399,9585,9774,9964,10156,10350,10545,10742,10941,11141,11343,11546,11751,11957,12165,12374,12585,12797,13010,13225,13441,13658,13876,14095,14315,14537,14759,14982,15206,15431,15657,15883,16111,16338,16567,16796,17025,17255,17486,17716,17947,18178,18410,18641,18873,19104,19335,19567,19798,20029,20260,20490,20720,20949,21178,21406,21634,21861,22087,22313,22537,22761,22983,23205,23425,23644,23862,24079,24294,24508,24720,24931,25140,25347,25553,25756,25958,26158,26356,26553,26746,26938,27128,27315,27500,27682,27863,28040,28215,28388,28557,28725,28889,29050,29209,29365,29518,29667,29814,29958,30098,30236,30370,30500,30628,30752,30873,30990,31104,31214,31321,31424,31524,31620,31712,31801,31885,31966,32044,32117,32187,32253,32315,32373,32427,32477,32523,32565,32603,32638,32668,32694,32716,32735,32749,32759,32765,32767,32765,32759,32749,32735,32716,32694,32668,32638,32603,32565,32523,32477,32427,32373,32315,32253,32187,32117,32044,31966,31885,31801,31712,31620,31524,31424,31321,31214,31104,30990,30873,30752,30628,30500,30370,30236,30098,29958,29814,29667,29518,29365,29209,29050,28889,28725,28557,28388,28215,28040,27863,27682,27500,27315,27128,26938,26746,26553,26356,26158,25958,25756,25553,25347,25140,24931,24720,24508,24294,24079,23862,23644,23425,23205,22983,22761,22537,22313,22087,21861,21634,21406,21178,20949,20720,20490,20260,20029,19798,19567,19335,19104,18873,18641,18410,18178,17947,17716,17486,17255,17025,16796,16567,16338,16111,15883,15657,15431,15206,14982,14759,14537,14315,14095,13876,13658,13441,13225,13010,12797,12585,12374,12165,11957,11751,11546,11343,11141,10941,10742,10545,10350,10156,9964,9774,9585,9399,9214,9031,8850,8670,8493,8318,8144,7972,7803,7635,7469,7306,7144,6984,6827,6671,6517,6366,6216,6069,5924,5780,5639,5500,5362,5227,5094,4963,4834,4708,4583,4460,4339,4220,4104,3989,3876,3766,3657,3550,3445,3342,3242,3143,3046,2950,2857,2766,2676,2589,2503,2419,2336,2256,2177,2100,2025,1952,1880,1810,1741,1674,1609,1545,1483,1423,1364,1306,1250,1196,1143,1091,1041,993,945,899,855,811,770,729,690,651,615,579,545,511,479,448,419,390,363,336,311,287,264,242,221,200,181,163,146,130,115,101,88,76,64,54,45,36,29,22,16,11,7,4,2,0};

volatile int fftReady = 0;

void processAudio() //put your signal processing code here.
{
  copyShortBuf(AudioC.inputLeft, AudioC.outputLeft, AudioC.adcBufferSize);
  copyShortBuf(AudioC.inputRight, AudioC.outputRight, AudioC.adcBufferSize);
  if(fftReady == 0) //only process the FFT once the print routine has asynchronously finished printing out. This will miss many frames of audio, but would be good for a spectrum analyzer, to give a feel for the spectrum
  {
    copyShortBuf(AudioC.inputLeft, fftBuf, I2S_DMA_BUF_LEN);
    windowSignal((int*) fftBuf, (int*) fftBuf, window, I2S_DMA_BUF_LEN);
    rfft(fftBuf,I2S_DMA_BUF_LEN,SCALE);
    psd(fftBuf, fftBuf, I2S_DMA_BUF_LEN);
    fftReady = 1;
  }
  
}


void psd(DATA* source, DATA* dest, int length) // computes magnitude of a vector.
{
  for(int i = 0; i < length; i+=2)
  {
      LDATA a = source[i]; 
      LDATA b = source[i+1];
      LDATA c = (a*a) + (b*b);
      dest[i/2] = c/2;
  }
  sqrt_16(dest, dest, length/2);
}


void windowSignal(int * source, int * destination, int *window, int length) //window the given signal.
{
	for(int i = 0; i < length; i++)
    {
      long a = source[i];
      long b = window[i];
      long c = ((long)(a * b)) >> 15;
      destination[i] = (int) c;
    }
}
void setup()
{
    int status;

    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();
    disp.setOrientation(1);   /* Set orientation of the LCD to horizontal */

    disp.print("Spectrum");
    delay(1500);
    disp.clear();    
    /* Audio library is configured for non-loopback mode. */
    status = AudioC.Audio(TRUE);
    pinMode(SW1, INPUT);
    pinMode(SW2, INPUT);
}

void loop() //Asynchronous non-signal processing code here.
{
  if(fftReady == 1)
  {
    xfOn();
    int displayMode = digitalRead(SW1);
    disp.plot((int*)fftBuf, 96, displayMode);
    fftReady = 0;
    xfOff();
  }
}
