/*! @file Finduntil.cpp
 *
 *  @brief Reads user input to Serial until string is found or user terminates
 *
 * Serial.findUntil() Demo
 *
 * This demo first reads a string of length 5 and then keeps reading data from
 * serial, until the User types the first string or the terminating string ("@")
 * or the serial.read() API times out waiting for the User input
 */

/// Read string from Serial until comparison string is found
void setup()
{
    Bool  result;
    char  targetString[50];
    Serial.begin(9600);
    Serial.println(" Serial.findUntil() Demo!");

    /*! Ask for the target string from the User */
    Serial.println("\r\nEnter the string to find (5 characters):");
	/// read 5 char string from Serial
    Serial.readBytes(targetString, 5);

	/// add string termination char
    targetString[5] = '\0';

    /*! Ask for the longer string in which to find the target string */
	Serial.println("Enter string in which the previous string should be looked for. Enter '@' to stop.");
    /*! Find target string in Serial string */
	result = Serial.findUntil(targetString, "@");
    if (result == TRUE)
    {
        Serial.println("\r\nString found");
    }
    else
    {
        Serial.println("\r\nTimeout: String not found");
    }

    Serial.println("\r\nEnd of Serial.finduntil() Demo!!");

    Serial.end();
}

void loop()
{
}