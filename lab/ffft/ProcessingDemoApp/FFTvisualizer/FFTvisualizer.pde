// Feel Free to edit these variables ///////////////////////////
String  xLabel = "Frequency";
String  yLabel = "Values";
String  Heading = "Arduino FFT";
String  URL = "01/02/2010";
float Vcc = 255.0;    // the measured voltage of your usb 
int NumOfVertDivisions=5;      // dark gray
int NumOfVertSubDivisions=10;  // light gray


int NumOfBars=64;    // you can choose the number of bars, but it can cause issues  
                    // since you should change what the arduino sends
                    

// if these are changed, backgroung image has problems 
// a plain background solves the problem
int ScreenWidth = 800, ScreenHeight=600;
/////////////////////////////////////////////////////////

//  Serial port stuff ///////////////////////
import processing.serial.*;
Serial myPort;        
boolean firstContact = false; 
int[] serialInArray = new int[NumOfBars];
int serialCount = 0;
///////////////////////////////////////////////

int LeftMargin=100;
int RightMArgin=80;
int TextGap=50;
int GraphYposition=80; 
float BarPercent = 0.4;

int value;

PFont font;
PImage bg;

int temp;
float yRatio = 0.58;
int BarGap, BarWidth, DivisounsWidth;
int[] bars = new int[NumOfBars];

void setup(){

  bg = loadImage("BG.jpg"); 

  /// NB SETTINGS ////////////////////////////////////////////////////////
  myPort = new Serial(this, Serial.list()[1], 57600); 
  ////////////////////////////////////////////////////////////////////////

  DivisounsWidth = (ScreenWidth-LeftMargin-RightMArgin)/(NumOfBars); 
  BarWidth = int(BarPercent*float(DivisounsWidth));
  BarGap = DivisounsWidth - BarWidth;

  size(ScreenWidth,ScreenHeight);
  font = createFont("Arial",12);
 
  textAlign(CENTER);
  textFont(font);
}

void draw(){

//  background(bg);     // My one used a background image, I've 
  background(250);      // commented it out and put a plain colour 

  //  Headings();           // Displays bar width, Bar gap or any variable. 
  Axis();
  Labels();
  PrintBars();
//  Line();
//  Dots(); 
}




// Send Recieve data //
void serialEvent(Serial myPort) {

  // read a byte from the serial port:
  int inByte = myPort.read();

  if (firstContact == false) {
    if (inByte == 'A') { 
      myPort.clear();          // clear the serial port buffer
      firstContact = true;     // you've had first contact from the microcontroller
      myPort.write('A');       // ask for more
    } 
  } 
  else {
    // Add the latest byte from the serial port to array:
    serialInArray[serialCount] = inByte;
    serialCount++;

    // If we have 6 bytes:
    if (serialCount > NumOfBars -1 ) {

      for (int x=0;x<NumOfBars;x++){
    
  bars[x] = int (yRatio*(ScreenHeight)*(serialInArray[x]/256.0));

}

      // Send a capital A to request new sensor readings:
      myPort.write('A');
      // Reset serialCount:
      serialCount = 0;
    }
  }
}

