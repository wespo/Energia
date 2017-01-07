/*! @file ReadBytesUntil.cpp
 *
 * @brief Reads string from Serial until terminating char
 * Serial.readBytesUntil() Demo
 *
 * This demo reads a string from serial, until the User types a string of
 * length 50 or types the terminating character ('@')
 */

/*! Setup Serial to read user input & print values */
void setup()
{
    /*! Initialize Buffer to hold String */
	char  readString[50];
	Serial.begin(9600);
    
	/*! Setup Serial to prompt user for input string */
	Serial.println("\r\nSerial.readBytesUntil() Demo!\r\n");

    Serial.println("\r\nEnter a string of maximum length 50 bytes. Press '@' to stop:");

    /*! readBytesUntil API reads string until terminating char or buffer length*/
	Serial.readBytesUntil('@', readString, 50);
	/*! Print output to Serial */
    Serial.print("You entered: ");
    Serial.write(readString);

    Serial.println("\r\n\r\nEnd of Serial.readbytesUntil() Demo!");

    Serial.end();
}

void loop()
{
}
