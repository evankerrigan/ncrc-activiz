// blink_slave.ino
// Refer to the "blink_master" example for use with this
#include <Wire.h>

const int SLAVE_ADDRESS = 1;
const int ledPin = 13;
char incomingByte = 0;

void setup() {  
  Wire.begin(SLAVE_ADDRESS);    // join I2C bus as a slave with address 1
  Wire.onReceive(receiveEvent); // register event
  
  pinMode(ledPin, OUTPUT);
}

void loop() {
}

void receiveEvent(int howMany)
{
  // receive one byte from Master
  incomingByte = Wire.read();
  
  // turn on or off LED according to the received data
  digitalWrite(ledPin, incomingByte);
}
