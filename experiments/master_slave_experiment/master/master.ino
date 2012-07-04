// blink_master.ino
// Refer to the "blink_slave" example for use with this
#include <Wire.h>

const int SLAVE_ADDRESS = 1;
const int ledPin = 13;
const int delay_ms = 400;

void setup() {  
  Wire.begin();         // join I2C bus as a Master  
  
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // step 1 - Blink Master's LED
  delay(delay_ms);
  digitalWrite(ledPin, HIGH);  // turn on Master's LED
  delay(delay_ms);  
  digitalWrite(ledPin, LOW);   // turn off Master's LED
  
  // step 2 - Blink Slave's LED
  delay(delay_ms);
  slaveLED(HIGH);              // turn on Slave's LED
  delay(delay_ms);  
  slaveLED(LOW);               // turn off Slave's LED
}

void slaveLED(byte value)
{
  // send data to slave
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(value);
  Wire.endTransmission();
}
