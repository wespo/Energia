/*! @file PrintFormat.cpp
 *
 * @brief Prints and int in various base representations
 * Serial Print Format Demo
 *
 * This demo prints an integer in various numeric format such as: binary,
 * decimal, octal and hexadecimal using Serial.println(value, format) API
 */

/// Print numbers to serial
void setup()
{
    long longVal;
    Serial.begin(9600);
    Serial.println("\r\nSerial Print Format Demo!\r\n");

	/// Binary representation uses BIN as format
    Serial.println(" Printing an integer '125' in Binary format using print(long, format) API");
    longVal = 125;
    Serial.println(longVal, BIN);

	/// Decimal representation uses DEC as format
    Serial.println("\r\n Printing an integer '0x7F' in Decimal format using print(long, format) API");
    longVal = 0x7F;
    Serial.println(longVal, DEC);

	/// Octal representation uses OCT as format
    Serial.println("\r\n Printing an integer '125' in Octal format using print(long, format) API");
    longVal = 125;
    Serial.println(longVal, OCT);

	/// Hexadecimal representation uses HEX as format
    Serial.println("\r\n Printing an integer '98765' in Hexdecimal format using print(long, format) API");
    longVal = 98765;
    Serial.println(longVal, HEX);

    Serial.println("\r\nEnd of Serial Print Format Demo!");

    Serial.end();
}

void loop()
{
}
