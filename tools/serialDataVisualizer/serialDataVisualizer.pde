// Graphing sketch

 // This program takes ASCII-encoded strings
 // from the serial port at 9600 baud and graphs them. It expects values in the
 // range 0 to 1023, followed by a newline, or newline and carriage return
 
 // Created 20 Apr 2005
 // Updated 18 Jan 2008
 // by Tom Igoe
 // This example code is in the public domain.
import processing.serial.*;

Serial myPort; 
int micro = 0;
int xPos = 1;
float lastInByte = 0;

void setup() 
{
  size(1000, 1023);
  String[] portList = Serial.list();
  println(portList);
  myPort = new Serial(this, portList[0], 9600);
  
  // Dont generate a serial Event unless you get a newline character
  myPort.bufferUntil('\n');
  background(0);   
  
}

void draw() 
{


}

void serialEvent(Serial myPort){
  String inString = myPort.readStringUntil('\n');

  if(inString != null){
    // trim off any whitespace
    inString = trim(inString);
    
    float inByte = float(inString);
    
    // convert to an int and map to the screen height
    inByte = map(inByte, 0, 1023, 0, height);
    
    //draw the line:
    stroke(127,34,255);
    line(xPos-1, height-lastInByte, xPos, height - inByte);

    lastInByte = inByte;
    
    if(xPos >= width){
      xPos = 0;
      background(0);
    } else {
      // increment the horizontal position
      xPos++;
    }
  }
}
