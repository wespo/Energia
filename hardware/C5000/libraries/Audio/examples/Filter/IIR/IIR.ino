/*! @file IIR.cpp
 *
 * @brief IIR Filter demo
 *
 * This demo reads data from codec audio IN, sends the data
 * to IIR filter and finally sends the filtered audio to the codec OUT
 * which can be listened on headphone.
 *
 * This program acts as a 8th order Low Pass Filter(LPF)
 * with cut-off frequency as 5kHz.
 *
 */

#include "Audio.h"
#include "iir.h"
#include "OLED.h"

#define ORDER (8)
// Filter coefficients - 8th order, 5k cutoff LPF, assumes 48k sample frequency
#pragma DATA_ALIGN(32);
int coeffs[7*(ORDER/2)] = {    
  14,-29811,14680, 313, 618, 305,2,
  14,-27198,11957, 362, 725, 362,2,
  14,-25488,10176, 352, 712, 360,2,
  14,-24650, 9302,1246,2493,1246,2
};

IIRClass IIRL, IIRR; //FIR Filter objects for both channels

// DMA Interrupt Service Routine
void processAudio()
{
  // Filter Left Audio Channel
  IIRL.filter(AudioC.inputLeft, AudioC.outputLeft, AudioC.adcBufferSize);
  // Filter Right Audio Channel
  IIRR.filter(AudioC.inputRight, AudioC.outputRight, AudioC.adcBufferSize);
}

// Initializes OLED and Audio modules
void setup()
{
    int status;
    int index;

    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, HIGH);

    //Initialize OLED module for status display
    disp.oledInit();
    disp.clear();    
    disp.flip();    
    disp.setline(1);

    /*! Clear the delay buffers, which will be used by the IIR filtering
       algorithm, These buffers need to be initialized to all zeroes in the
       beginning of the IIR filtering algorithm */
    IIRL.init(coeffs, ORDER);
    IIRR.init(coeffs, ORDER);             

    /*! Audio library is configured for non-loopback mode. Gives enough time for
       processing in ISR */
    status = AudioC.Audio(TRUE);
    if (status == 0)
    {
    	disp.print("IIR Filter ON");
    }
}

void loop()
{
}
