/*
 * Analog Read
 * Reads analog pin data from A1 pin contnuosly and displays it on the serial
 * console.
 */

void setup()
{
	Serial.begin(9600);
    Serial.println("\r\nThe Read data is:");
}

void loop()
{
    // Read anlog pin and send the value to serial monitor	
    Serial.println(analogRead(A1), HEX);
}

