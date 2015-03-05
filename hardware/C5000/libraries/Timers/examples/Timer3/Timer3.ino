/*
 * Timer instance 3 demo:
 * This test verifies whether the GPT3 is decrementing the counter or not.
 * GPT3 is configured with a count value of 0x10000 and pre-scaler divider
 * value of 2. GPT3 is started and counter value is read. After few
 * cycles of delay GPT3 counter value is read again. Both the counter values
 * are compared to verify whether the second count value is less
 * than the first counter value or not and the test passes if it is true.
 *
 * This sketch dmonstrates usage of function Timer.setPeriod
 */

#include <Timers.h>

void setup() {
    unsigned long           timeCnt1;
    unsigned long           timeCnt2;
    long                    period;
    volatile unsigned short delay;
    Serial.begin(9600);   
    Serial.print("\nGPT - Example for setPeriod() \n");
        
    // Open the GPT module for GPT 2 instance 
    Timer.selectTimer(GPT2);

    // Configure GPT period 
    period = 0x10000;
    Timer.setPeriod(period);

    // Start the Timer 
    Timer.start();

    // Read the Timer Count 
    timeCnt1 = Timer.read();

    // Give Some Delay 
    delayMilliseconds(100);

    // Read the Timer Count Again 
    timeCnt2 = Timer.read();

    // Compare the timer count to verify whether the timer is counting or not 
    if (timeCnt2 >= timeCnt1)
    {
        Serial.print("GTP Time Count Compare Failed\n");
    }
    else
    {
        Serial.print("GTP Time Count Compare Successful\n");
    }

    // Stop The Timer 
    Timer.stop();

    // Close the GPT module 
    Timer.close();

    Serial.print("\nEnd of Example!\n");
}

void loop() {
}
