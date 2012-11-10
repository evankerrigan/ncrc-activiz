#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>

#include <Wire.h>

#define IR_AUDIO  0 // ADC channel to capture
#define PIN_DISTANCE_IN A3

#define SLAVE_ADDRESS 1

// Using Classes from ledcontroller library
using LedController::Color; 
using LedController::LedStrip;
using LedController::PatternChangingColorColumn;
using LedController::PatternHourGlass;
using LedController::PatternBarPlotToBarPlot;
using LedController::Interval;

//Colors Used
//Color red(0xFF0000);
//Color prettyblue(0x6FBAFC);
Color oceanicblue(0x00FF80);

Color algaegreen(0x80FF00);
Color darkgreen(0x00FF00);
Color purple1(0x800080);
Color purple2(0x700075);
Color purple3(0x600070);
//Color purple4(0x500075);

// Assign PINs to Led Strips
LedStrip ledStrips[] = {LedStrip(PIN_LED1_OUT_SDI, PIN_LED1_OUT_CKI),
                       LedStrip(PIN_LED2_OUT_SDI, PIN_LED2_OUT_CKI),
                       LedStrip(PIN_LED3_OUT_SDI, PIN_LED3_OUT_CKI),
                       LedStrip(PIN_LED4_OUT_SDI, PIN_LED4_OUT_CKI)};
byte leds[4][32];

// Interval for Controller
//Interval oneSec = Interval(1000);
//byte currentTimeSec = 0;
//byte currentTimeMin = 0;

// Pattern Sets
//PatternChangingColorColumn patCCC = PatternChangingColorColumn(purple1); //Pattern for Led strip 1
//
//PatternHourGlass patHourGlassForSec = PatternHourGlass(oceanicblue, algaegreen, darkgreen);
//PatternHourGlass patHourGlassForMin = PatternHourGlass(oceanicblue, algaegreen, darkgreen);
//PatternHourGlass patHourGlassForPastHour = PatternHourGlass(oceanicblue, algaegreen, darkgreen, false);
////
//PatternBarPlotToBarPlot patBarPlotForHourAni = PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen);
//PatternBarPlotToBarPlot patBarPlotForHourAniRemain = PatternBarPlotToBarPlot(0, 0, oceanicblue, oceanicblue);

const long referenceMv = 5000;

void setup()
{  
  
  pinMode(PIN_IR_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);
  pinMode(PIN_DISTANCE_IN, INPUT);
  
  // Initialize ADC
  Serial.begin(9600);
  
  //Wire.begin();  // Join I2C bus as a master
  
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS; i++){
    ledStrips[i].setup();
    ledStrips[i].clear();
    ledStrips[i].send();
  }
  
  for(byte i = 0; i < 4; i++){
    for(byte j=0; j < 32; j++){
      leds[i][j] = i * j;
    }
  }
  
  // Feed fake data for the hour glasses which stored the human voice information in the past hours
  //patHourGlassForPastHour.setActualValueBeingStored(29);
  //patHourGlassForMin.setActualValueBeingStored(15);
  
  // Reset Barplot remain
  //  patBarPlotForHourAniRemain.setStartPosition(0);
  
  delay(1000);
}


byte r = 0;
byte pat_number = 0;
boolean is_increase = true;
byte num_fireworks = 0;
boolean is_fireworks[4][32];
byte fireworks_color[4][32];

void loop()
{ 
  // snippet from Arduino Cookbook pp.195-195
  int val = analogRead(PIN_DISTANCE_IN);
  int mV = (val * referenceMv) / 1023;
  Serial.print(mV);
  Serial.print(",");
  int cm = getDistance(mV);
  Serial.println(cm);
  
  //if a user is close to the distance sensor, release a firework at a random position
  if(cm <= 80){
    byte i = random(4);
    byte j = random(32);
    is_fireworks[i][j] = true;
  }
  
  if(r == 250 )
    is_increase = false;
  else if (r == 0){
    is_increase = true;
    pat_number++;
    if(pat_number > 3)
      pat_number = 0;
  }
  
  if(is_increase)
    r = r + 2;
  else
    r = r - 2;

  
  //*****CONTROLLER & RENDERER BEGIN*************************
  /* Render background */
  // Clear LED strips color
    for(byte i=0; i < NUM_LED_STRIPS; i++){
      ledStrips[i].clear();
    }
 
 
  
  for(byte i=0; i < NUM_LED_STRIPS; i++){  
    Color* colors = ledStrips[i].getColors();
    for(byte j=0; j < 32; j++){
      if(is_fireworks[i][j]){
         colors[j].setChannelValues(fireworks_color[i][j], 10, 10 );
         fireworks_color[i][j]++;
         if(fireworks_color[i][j] == 0){
           is_fireworks[i][j] = false;
         } 
      } else {
      if(pat_number==0)
        colors[j].setChannelValues(r, 125, r );
      else if(pat_number==1)
        colors[j].setChannelValues(r/2, 125+r/2, r);
      else if(pat_number==2)
        colors[j].setChannelValues(0, 125-(r/2), r/2);
      else if(pat_number==3)
        colors[j].setChannelValues(r, 125-(r/2), 0);

    }
    ledStrips[i].send();
  }
  Serial.println(pat_number);
  
  // I2C Communication
//  if( state == S_WAITING_RESPONSE){
//    byte slaveFinished = requestSlaveState();
//    Serial.print("debug,slaveFinished=");
//    Serial.println(slaveFinished);
//    if(slaveFinished){
//      state = S_HOUR_ANIMATION;
//      hourAnimationForMasterHasStarted = true;
//    }
//  } 
  
}
const int TABLE_ENTRIES = 12;
const int firstElement = 250; // first entry is 250 mV
const int INTERVAL = 250; // millivolts between each element
static int distance[TABLE_ENTRIES] = {150,140,130,100,60,50,40,35,30,25,20,15};

int getDistance(int mV)
{
  if( mV > INTERVAL * TABLE_ENTRIES-1 )
      return distance[TABLE_ENTRIES-1];
  else {
    int index = mV / INTERVAL;
    float frac = (mV % 250) / (float)INTERVAL;
    return distance[index] - ((distance[index] - distance[index+1]) * frac);
  }
}


void sendEvent(byte value)
{
  
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(value);
  Wire.endTransmission();
}

byte requestSlaveState()
{
  byte slaveFinished = 0;
  Wire.requestFrom(SLAVE_ADDRESS,1);
  if(Wire.available()){
    slaveFinished = Wire.read();
    Serial.print("slaveFinished()=");
    Serial.println(slaveFinished);
  }
  return slaveFinished;
}
