/*
  SDRAM DSP API example sketch.
  This sketch verifies SDRAM operation by writing 1KB of data to
  SDRAM memory starting from first location. Same memory locations
  are read and compared with written data to confirm the success
  of read and write operation. Data written and read will be
  displayed on serial monitor.

  Note: This example is specific to C5517 device, cannot be executed
  on C5535 device
*/

#include "SDRAM.h"
#include "Serial_lib.h"

#define SDRAM_BUF_SIZE     (512)

unsigned short  gSdramWrBuf[SDRAM_BUF_SIZE];
unsigned short  gSdramRdBuf[SDRAM_BUF_SIZE];

void setup()
{
    int              result;
    unsigned short   index;
    Serial.begin(9600);
    Serial.println("SDRAM Read/Write Demo!\n");

    for (index = 0; index < SDRAM_BUF_SIZE; index++)
    {
        gSdramWrBuf[index] = rand();
    }

    /* Initializes HW controller for SDRAM access */
    result = SDRAM.init();
    if (result == 0)
    {
        Serial.println("Writing 1KB of Data to SDRAM...");

        /* Writes 1KB of data to SDRAM */
        result = SDRAM.write(gSdramWrBuf, SDRAM_START_ADDR, SDRAM_BUF_SIZE);
        if (result == 0)
        {
            Serial.println("SDRAM Write Operation Successful!");
            Serial.println("\nReading 1KB of Data from SDRAM...");

            /* Reads 1KB of data from SDRAM */
            result = SDRAM.read(gSdramRdBuf, SDRAM_START_ADDR, SDRAM_BUF_SIZE);
            if (result == 0)
            {
                Serial.println("SDRAM Read Operation Successful!\n");

                for (index = 0; index < SDRAM_BUF_SIZE; index++)
                {
                    if (gSdramWrBuf[index] != gSdramRdBuf[index])
                    {
                        Serial.println("SDRAM Buffer Mismatch!");
                        break;
                    }
                }

                if (index == SDRAM_BUF_SIZE)
                {
                    Serial.println("\n\nSDRAM Read and Write Buffers are Same!");
                    Serial.println("SDRAM Data Transfer Successful!!");
                }
                else
                {
                    Serial.println("\n\nSDRAM Data Transfer Failed!!");
                }
            }
        }
    }

    Serial.println("\nEnd of SDRAM Read/Write Demo!!\n");
    Serial.end();
}

void loop()
{
}
