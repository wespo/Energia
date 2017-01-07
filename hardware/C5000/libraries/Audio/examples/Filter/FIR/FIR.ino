/*! @file FIR.cpp
 *
 * @brief FIR Filter demo low-pass filters audio input
 *
 * Reads data from codec audio IN, sends the data to FIR filter and 
 * finally sends the filtered audio to the codec OUT which can be 
 * listened on headphone.
 *
 * Filter is configured as a Low Pass Filter(LPF) with cut-off 
 * frequency as 2kHz.
 */

#include "Audio.h"
#include "fir.h"
#include "OLED.h"

FIRClass FIRL, FIRR; //FIR Filter objects for both channels

// Length of the Coefficient Vector
#define FILTER_LENGTH (201)

// Filter coefficients with 201 taps, 2kHz cutoff
int coeffs[FILTER_LENGTH] = {
        7,      6,      4,      2,      0,     -2,     -5,     -7,     -9,
      -11,    -12,    -12,    -12,    -10,     -8,     -4,      0,      5,
       10,     15,     19,     23,     25,     26,     24,     21,     16,
        9,      0,    -10,    -20,    -30,    -39,    -46,    -50,    -51,
      -49,    -42,    -31,    -17,      0,     19,     38,     57,     73,
       86,     93,     94,     89,     76,     56,     31,      0,    -34,
      -68,   -100,   -129,   -150,   -163,   -164,   -154,   -132,    -97,
      -53,      0,     58,    117,    173,    222,    260,    282,    286,
      269,    231,    172,     94,      0,   -104,   -212,   -318,   -412,
     -488,   -538,   -555,   -533,   -467,   -356,   -200,      0,    239,
      510,    805,   1114,   1424,   1725,   2005,   2252,   2455,   2607,
     2701,   2733,   2701,   2607,   2455,   2252,   2005,   1725,   1424,
     1114,    805,    510,    239,      0,   -200,   -356,   -467,   -533,
     -555,   -538,   -488,   -412,   -318,   -212,   -104,      0,     94,
      172,    231,    269,    286,    282,    260,    222,    173,    117,
       58,      0,    -53,    -97,   -132,   -154,   -164,   -163,   -150,
     -129,   -100,    -68,    -34,      0,     31,     56,     76,     89,
       94,     93,     86,     73,     57,     38,     19,      0,    -17,
      -31,    -42,    -49,    -51,    -50,    -46,    -39,    -30,    -20,
      -10,      0,      9,     16,     21,     24,     26,     25,     23,
       19,     15,     10,      5,      0,     -4,     -8,    -10,    -12,
      -12,    -12,    -11,     -9,     -7,     -5,     -2,      0,      2,
        4,      6,      7
};

void processAudio() //put your signal processing code here.
{
  // Filter Left Audio Channel
  FIRL.filter(AudioC.inputLeft, AudioC.outputLeft, AudioC.adcBufferSize);
  // Filter Right Audio Channel
  FIRR.filter(AudioC.inputRight, AudioC.outputRight, AudioC.adcBufferSize);
}

void setup()
{
    int status;

    //Initialize OLED module for status display	
    disp.oledInit();
    disp.clear();
    disp.flip();    
    disp.setline(1);

    /* Set up FIR filter for each chanel */
    FIRL.init(coeffs, FILTER_LENGTH);
    FIRR.init(coeffs, FILTER_LENGTH);
    
    /* Audio library is configured for non-loopback mode. */
    status = AudioC.Audio(TRUE);

    if (status == 0) //on success, report ready on OLED display.
    {
        disp.print("FIR Filter ON");
    }        
}

void loop() //Asynchronous non-signal processing code here.
{
  
}
