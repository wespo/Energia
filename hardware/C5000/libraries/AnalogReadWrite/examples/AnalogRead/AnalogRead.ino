/*! @file AnalogRead.cpp
 * 
 * @brief Read analog pin data
 *
 * Reads analog pin data from A0 pin contnuosly and displays it on the serial
 * console.
 */

/// Set up Serial Display 
void setup()
{
	Serial.begin(9600);
    Serial.println("\r\nThe Read data is:");
}

/// Read analog pin and send the value to serial monitor	
void loop()
{
    // Read anlog pin and send the value to serial monitor	
    Serial.println(analogRead(A1), HEX);
}

