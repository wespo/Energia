/*! @file ReadBytes.cpp
 *  
 *  @brief Reads a string of length 5 from the Serial and displays it back on the Serial
 * 
 * Serial.readBytes() Demo
 *
 */

/*! Setup Serial to read user input & print values */
void setup()
{
    /*! Initialize buffer to hold user input */
	char  readString[50];
	Serial.begin(9600);
    /*! Setup Serial to prompt user for input string */
	Serial.println("\r\nSerial.readBytes() Demo!\r\n");

    Serial.println("\r\nEnter a string of length 5 bytes:");

    /*! Read string from serial */
	Serial.readBytes(readString, 5);
    Serial.print("The string you entered is: ");
    /*! Print string to serial */
	Serial.write(readString, 5);

    Serial.println("\r\n\r\nEnd of Serial.readbytes() Demo!!");

    Serial.end();
}

void loop()
{
}
