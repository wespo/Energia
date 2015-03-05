/*
 * Watchdog timer demo:
 * Configures watchdog timer, services it for some cycles,
 * then stops servicing the watchdog which should reset the
 * DSP shield board
 */

#include <Timers.h>

void setup() {
    WDT_Config              hwConfig;
    unsigned long           counter;
    volatile unsigned short delay;
    Serial.begin(9600);
    hwConfig.counter  = 0xFFFF;
    hwConfig.prescale = 0x00FF;

    // Open the WDTIM module 
    Timer.setWdt(0, hwConfig);

    // Start the watch dog timer 
    Timer.startWdt();

    delayMilliseconds(10);

    // Stop the watch dog timer 
    Timer.stopWdt();

    // Start the watch dog timer 
    Timer.startWdt();

    counter = 0;
    for (counter=0; counter<0x105; counter++)
    {
        /* Servicing the watchdog timer for 256 cycles
           program should be running properly as long as
           watchdog is serviced */
        if (counter < 0x100)
        {
            Timer.serviceWdt();
            Serial.print("\r\nWDT Service - ");
            Serial.println((long)counter);
        }
        else
        {
            /* Watchdog servicing is stopped. 
               DSP shield shall be reset soon and message
               display on serial monitor shall be stopped */
            Serial.print("\r\nWDT Out of Service - ");
            Serial.println((long)counter);
        }

        counter++;
    }

    Serial.print("End of Watchdog Timer Demo\r\n");
}

void loop() {
}
