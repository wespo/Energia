#include "SPI.h"
void setup()
{
  // put your setup code here, to run once:
  SPI.begin();
  pinMode(LED0, OUTPUT);
}

unsigned long data_count = 0x20;
bool ledOnOff = false;
void loop()
{
  // put your main code here, to run repeatedly:
  unsigned long data_count2 = data_count;
  SPI.write(&data_count2, 1);
  data_count++;
  if(data_count == 0x7F)
  {    
    data_count2 = '\n';
    SPI.write(&data_count2, 1);
    delay(10);
    digitalWrite(LED0, ledOnOff);
    ledOnOff = !ledOnOff;
    data_count = 0x20;
  }
}