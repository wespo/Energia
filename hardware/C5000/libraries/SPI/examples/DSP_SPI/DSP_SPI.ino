/*! @DSP_SPI.cpp
 *
 * @brief SPI and Arduino Communication demo: Receives and acknowledges data from Arduino
 *
 * This code should be flashed to the DSP Shield board
 *
 * Receives data (value 0-2) from the Arduino and blinks that particular LED
 *
 * If value received is 0, LED0 will be ON for 500 msecs
 * If value received is 1, LED1 will be ON for 500 msecs
 * If value received is 2, LED2 will be ON for 500 msecs
 *
 */

#include "SPI.h"

unsigned long buffer;

unsigned int cmd[2];
int ledValueRcvd  = 0;
int ioExpPinState = 0;

/*! Initializes the SPI module and enables the communication link (DSP_SPI_EN pin),
 * for communication with the Arduino board
 */
void setup()
{
    cmd[0] = 0x06;                       /*! Command to configure IO expander pins as Output pins */
    cmd[1] = 0x00;
    Wire.beginTransmission(0x21);        /*! 0x21 is the address of IO expander for which LEDs and DSP_SPI_EN pins are connected */
    Wire.write((unsigned int *)cmd, 2);  /*! Configures IO expander port 0 pins as output */
    Wire.endTransmission();
    delay(1000);                         /*! Delay of 1sec */

    cmd[0] = 0x02;
    cmd[1] = 1 << 7;                     /*! Enable DSP_SPI_EN pin, to configure DSP Shield as Master and
                                            Arduino as slave, for DSP-Arduino SPI communication          */
    Wire.beginTransmission(0x21);		/*! 0x21 is the address of IO expander for which LEDs and DSP_SPI_EN pins are connected */
    Wire.write((unsigned int *)cmd, 2); /*! Configures IO expander port 0 pins as output */
    Wire.endTransmission();

    SPI.begin();                         /*! Initialize DSP SPI peripheral */
}

/*! Main loop - Receives the value from the Arduino through SPI and blinks the
 * respective LED
 */
void loop()
{
    buffer = 0xFFFFFFFF;

    if (0 == SPI.read(&buffer, 1)) /// read length 1 buffer from SPI
    {

        if (0xFFFFFFFF != buffer)
        {
            ledValueRcvd = (int)((buffer >> 16) & 0xF); /*! Received value will be in upper 16 bits */

            /*! Below logic ensures 3 LEDs connected to IO expander are toggled in sequence
               LED0 (pin 0) - Will be ON when 'ledValue' is '0'
               LED1 (pin 1) - Will be ON when 'ledValue' is '1'
               LED2 (pin 2) - Will be ON when 'ledValue' is '2'
             */
            ioExpPinState = ((ledValueRcvd / 2) << 6) | ((ledValueRcvd % 2) << 1) | (!ledValueRcvd);

            /*! Enable DSP_SPI_EN pin, to configure DSP Shield as Master and Arduino as slave for SPI.
               Toggling only LEDs will affect DSP_SPI_EN pin of the IO Expander 0x21
             */
            ioExpPinState |= (1 << 7);
            cmd[0] = 0x02;
            cmd[1] = ioExpPinState;
            Wire.beginTransmission(0x21);
            Wire.write((unsigned int *)cmd, 2);
            Wire.endTransmission();

            delay(500);  /*! Giving delay of 500 msecs between blinking of two LEDs */
        }
    }
}