/*! @file Read.cpp
 *
 * @brief Reads & Writes char on Serial display
 *
 * Serial Read and Write Demo
 *
 * This demo read a single character from the Serial and displays it back on
 * the Serial
 */

/*! Setup Serial to read user input & print values */
void setup()
{
    char  charVal;
	Serial.begin(9600);
	/*! Setup Serial to prompt user for input */
    Serial.println("\r\nSerial Read and Write Demo!\r\n");

    Serial.println("Enter a Character (1 byte):");

    /*! Read Serial char value */
	charVal = Serial.read();
	/*! Print Value to Serial from user input */
    Serial.print("You entered: ");
    Serial.println(charVal);

    Serial.println("\r\nEnd of Serial Read and Write Demo!");

    Serial.end();
}

void loop()
{
}
