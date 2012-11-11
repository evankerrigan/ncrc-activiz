#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#define A_MINUTE 10
#define A_HOUR 10
// Debug
boolean debug = DEBUG_FLAG;
//*****

// Master and Slave Protocal
#define EVENT_HA_START 0  //HA = Hour Animation
#define EVENT_HA_LAST_ROD 1
#define SLAVE_ADDRESS 1

#define S_NORMAL 0
#define S_HOUR_ANIMATION_HEAD 1
#define S_HOUR_ANIMATION_TAIL 2
byte state = 0;

bool finishFirstFourRods = 0;

// Using Classes from ledcontroller library
using LedController::Color; 
using LedController::LedStrip;
using LedController::PatternChangingColorColumn;
using LedController::PatternHourGlass;
using LedController::PatternBarPlotToBarPlot;
using LedController::Interval;

// Colors Used
//Color red(0xFF0000);
Color prettyblue(0x6FBAFC);
Color oceanicblue(0x00FF80);
Color skyblue(0x00FFFF);
//Color dye(0x6F6F10);
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
                       LedStrip(PIN_LED4_OUT_SDI, PIN_LED4_OUT_CKI),
                       LedStrip(PIN_LED5_OUT_SDI, PIN_LED5_OUT_CKI)};

// Interval for Controller
byte currentTimeSec = 0;
byte currentTimeMin = 0;

// Hour Animation States
#define FIRST_MOVEMENT_INI 0
#define FIRST_MOVEMENT_UPDATE 1
#define MID_MOVEMENT_INI 2
#define MID_MOVEMENT_UPDATE 3
#define LAST_MOVEMENT_INI 4
#define LAST_MOVEMENT_UPDATE 5
#define FINISH 6
#define WAIT 7

// Hour Animation Internal States for Middle Rods
#define DOWN 0
#define UP 1
#define REMAIN 2

// LED strips State
#define NORMAL 0
#define HOUR_ANIMATION_DOWN 1
#define HOUR_ANIMATION_UP 2
#define HOUR_ANIMATION_REMAIN 3

// Control flags
bool hourAnimationHasStarted = false;  // Master --> Slave
byte hourAnimationState = FIRST_MOVEMENT_INI;
byte hourAnimationToken = NUM_LED_STRIPS_SLAVE -1; //The Token starts from the last led strip
byte hourAnimationMidMovState = DOWN; //DOWN or UP
byte ledStripsState[NUM_LED_STRIPS_SLAVE];
byte tempValueForHourGlass;
byte actualValueForLastRod = 0; // Send from Master
// Pattern Sets

PatternHourGlass patHourGlassesForPastHours[] = {PatternHourGlass(oceanicblue, algaegreen, darkgreen, false),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen, false),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen, false),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen, false),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen, false),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen, false)};

PatternHourGlass tempHourGlass = PatternHourGlass(oceanicblue, algaegreen, darkgreen, false);

PatternBarPlotToBarPlot patBarPlotsForHourAni[] = {PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen),
                                                  PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen),
                                                  PatternBarPlotToBarPlot(0, 0, oceanicblue, algaegreen)};

// Temp Value
byte tempIndicator = 0;
Color tempBgColor;
Color tempBarColor;



void setup()
{  
  // Initialize ADC
  Serial.begin(9600);
  
  // I2C Communication, Master - Slave Mode
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    ledStrips[i].setup();
    ledStrips[i].clear();
    ledStrips[i].send();
  }
  
  // Feed fake data for the hour glasses which stored the human voice information in the past hours
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    patHourGlassesForPastHours[i].setActualValueBeingStored(random(20)+20);
  }
  
  // Reset barplot
  patBarPlotsForHourAni[REMAIN].setStartPosition(0);
  
  //Serial.println("ProgramStart");
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  delay(1000);
}

void loop()
{
  
  /* Render background */
  // Clear LED strips color
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
      ledStrips[i].clear();
  }
    
  // Update the sinosoidal background patterns for all the LED strips inherited PatternSineWave class
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    patHourGlassesForPastHours[i].updateSine();
  }
 
  /* finish render background*/

  // 
  
  // Hour Animation
  if(hourAnimationHasStarted){
    switch(hourAnimationState){
      case FIRST_MOVEMENT_INI:  //DOWN
        //Ini everything
        hourAnimationToken = NUM_LED_STRIPS_SLAVE -1; //The Token starts from the last led strip
        hourAnimationMidMovState = DOWN; //DOWN or UP
      
        debug && Serial.println("FIRST_MOVEMENT_INI");
        debug && Serial.print(" indicator = ");
        debug && Serial.println(patHourGlassesForPastHours[hourAnimationToken].getIndicator());
        patBarPlotsForHourAni[DOWN].restart();
        patBarPlotsForHourAni[DOWN].setStartPosition(patHourGlassesForPastHours[hourAnimationToken].getIndicator());
        patBarPlotsForHourAni[DOWN].setEndPosition(0);
        patBarPlotsForHourAni[DOWN].setBgColor(patHourGlassesForPastHours[hourAnimationToken].getBgColor());
        patBarPlotsForHourAni[DOWN].setBarColor(patHourGlassesForPastHours[hourAnimationToken].getIndicatorColor());
        ledStripsState[hourAnimationToken] = HOUR_ANIMATION_DOWN;
        hourAnimationState = FIRST_MOVEMENT_UPDATE;
        break;
      case FIRST_MOVEMENT_UPDATE:  //DOWN
        debug && Serial.println("FIRST_MOVEMENT_UPDATE");
        debug && Serial.print(hourAnimationToken);
        patBarPlotsForHourAni[DOWN].update();
        debug && Serial.print("isExpired=");
        debug && Serial.println(patBarPlotsForHourAni[0].isExpired());
        if(patBarPlotsForHourAni[DOWN].isExpired()){
        
          ledStripsState[hourAnimationToken] = HOUR_ANIMATION_REMAIN;
          hourAnimationState = MID_MOVEMENT_INI;
          hourAnimationToken--;
        }
        //patBarPlotForHourAni.apply(ledStrips[3].getColors());
        break;
      case MID_MOVEMENT_INI:
        if(hourAnimationMidMovState == DOWN){ // Leds go down
          debug && Serial.println("MID_MOVEMENT_INI DOWN");
          debug && Serial.print(hourAnimationToken);

          tempValueForHourGlass = patHourGlassesForPastHours[hourAnimationToken].getActualValueBeingStored();
          
          debug && Serial.print("tempValue = ");
          debug && Serial.println(tempValueForHourGlass);
          
          tempIndicator = patHourGlassesForPastHours[hourAnimationToken].getIndicator();
          tempBgColor = patHourGlassesForPastHours[hourAnimationToken].getBgColor();
          tempBarColor =  patHourGlassesForPastHours[hourAnimationToken].getIndicatorColor();
          ledStripsState[hourAnimationToken] = HOUR_ANIMATION_DOWN;
          patBarPlotsForHourAni[DOWN].restart();
          patBarPlotsForHourAni[DOWN].setStartPosition(tempIndicator);
          patBarPlotsForHourAni[DOWN].setEndPosition(0);
          patBarPlotsForHourAni[DOWN].setBgColor(tempBgColor);
          patBarPlotsForHourAni[DOWN].setBarColor(tempBarColor);
          
          hourAnimationState = MID_MOVEMENT_UPDATE;
        } else { // LEDS go up
          debug && Serial.println("MID_MOVEMENT_INI UP");
          debug && Serial.print(hourAnimationToken);
          ledStripsState[hourAnimationToken] = HOUR_ANIMATION_UP;
          patBarPlotsForHourAni[UP].restart();
          patBarPlotsForHourAni[UP].setStartPosition(0);
          patBarPlotsForHourAni[UP].setEndPosition(tempIndicator);
          patBarPlotsForHourAni[UP].setBgColor(tempBgColor);
          patBarPlotsForHourAni[UP].setBarColor(tempBarColor);
          
          hourAnimationState = MID_MOVEMENT_UPDATE;
        }
        break;
      case MID_MOVEMENT_UPDATE:
        debug && Serial.println("MID_MOVEMENT_UPDATE");
        debug && Serial.print(hourAnimationToken);
        bool isExpired;
        if(hourAnimationMidMovState == DOWN){  // LED Go DOWN
          patBarPlotsForHourAni[DOWN].update();
          isExpired = patBarPlotsForHourAni[DOWN].isExpired();
        }
        else { // LED GO UP
          patBarPlotsForHourAni[UP].update();
          isExpired = patBarPlotsForHourAni[UP].isExpired();
        }
        
        if(isExpired){ // LED Go UP
          if(/*NEED TO MOVE TO NEXT ROD && NEXT ROD = LAST ONE*/ hourAnimationMidMovState == UP && hourAnimationToken ==  1)
          {
            debug && Serial.println("GO LAST ROUND");
            debug && Serial.print(hourAnimationToken);
            
            patHourGlassesForPastHours[hourAnimationToken].setActualValueBeingStored(tempValueForHourGlass);
            ledStripsState[hourAnimationToken] = NORMAL;
            hourAnimationState = WAIT;
            finishFirstFourRods = true;
            
          } else if(/*FROM DOWN TO UP*/ hourAnimationMidMovState == DOWN){
            debug && Serial.println("GO FROM DOWN TO UP");
            debug && Serial.print(hourAnimationToken);
            
            //State Transition Tasks
            ledStripsState[hourAnimationToken] = HOUR_ANIMATION_REMAIN;
            hourAnimationState = MID_MOVEMENT_INI;
            hourAnimationMidMovState = UP;
            hourAnimationToken++;
            
          } else if(/*NEED TO MOVE TO NEXT ROD*/ hourAnimationMidMovState == UP){
              debug && Serial.println("GO NEXT ROUND");
              debug && Serial.print(hourAnimationToken);
            
            
              // State Transition Task
              hourAnimationState = MID_MOVEMENT_INI;
              hourAnimationMidMovState = DOWN;
              // Change the current rod back to hourglass
              ledStripsState[hourAnimationToken] = NORMAL;
              debug && Serial.print("tempValue= ");
              debug && Serial.println(tempValueForHourGlass);
              patHourGlassesForPastHours[hourAnimationToken].setActualValueBeingStored(tempValueForHourGlass);
              hourAnimationToken -= 2;
          }
          
        }
        break;
       case WAIT:
         debug && Serial.println("WAIT");
         debug && Serial.print("finishFirstFourRods=");
         debug && Serial.print(finishFirstFourRods);
         
         if(state == S_HOUR_ANIMATION_TAIL){
           hourAnimationState = LAST_MOVEMENT_INI;
         }
         break;
       case LAST_MOVEMENT_INI:
         debug && Serial.println("LAST_MOVEMENT_INI");
         hourAnimationToken = 0;
         patBarPlotsForHourAni[UP].restart();
         patBarPlotsForHourAni[UP].setStartPosition(0);
         
         tempIndicator = actualValueForLastRod % 30;
         
         Serial.print("indicator= ");
         Serial.println(tempIndicator);
         
         tempBarColor = (actualValueForLastRod > 30) ? darkgreen : algaegreen;
         
         patBarPlotsForHourAni[UP].setEndPosition(tempIndicator);  //test
         patBarPlotsForHourAni[UP].setBgColor(tempBgColor);  //test
         patBarPlotsForHourAni[UP].setBarColor(tempBarColor);  //test
         ledStripsState[hourAnimationToken] = HOUR_ANIMATION_UP;
         hourAnimationState = LAST_MOVEMENT_UPDATE;
         break;
       case LAST_MOVEMENT_UPDATE:
         debug && Serial.println("LAST_MOVEMENT_UPDATE");
         patBarPlotsForHourAni[UP].update();
         if(patBarPlotsForHourAni[UP].isExpired()){
           ledStripsState[hourAnimationToken] = NORMAL;
           patHourGlassesForPastHours[hourAnimationToken].setActualValueBeingStored(actualValueForLastRod); //test
           hourAnimationHasStarted = false;
           state = S_NORMAL;
           
           // Reset state
           hourAnimationState = FIRST_MOVEMENT_INI;
           debug && Serial.println("Finish HA");
         }
         break;
       default:  //Do nothing
         break;
    }
    
  } else {
  
  }
  
  // Put all the updated Colors onto the LED strips
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    switch(ledStripsState[i]){
      case NORMAL:
        patHourGlassesForPastHours[i].apply(ledStrips[i].getColors());
        break;
      case HOUR_ANIMATION_UP:
        patBarPlotsForHourAni[UP].apply(ledStrips[i].getColors());
        break;
      case HOUR_ANIMATION_DOWN:
        patBarPlotsForHourAni[DOWN].apply(ledStrips[i].getColors());
        break;
      case HOUR_ANIMATION_REMAIN:
        patBarPlotsForHourAni[REMAIN].apply(ledStrips[i].getColors());
        break;        
      default:
        patHourGlassesForPastHours[i].apply(ledStrips[i].getColors());
        break;
    }
    
  }
  
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){  
    ledStrips[i].send();
  }

 

}


/* I2C Communication, State Transition */
void receiveEvent(int howMany)
{
  Serial.println("Receive Event from Master");
  Serial.print("State = ");
  Serial.println(state);
  // receive on byte from master
  byte incomingByte = Wire.read();
  if(state == S_NORMAL){
    state = S_HOUR_ANIMATION_HEAD;
    actualValueForLastRod = incomingByte;

    Serial.print("Receive=");
    Serial.println(actualValueForLastRod);
  
    hourAnimationHasStarted = true;
    
  } 
}

void requestEvent()
{
  Serial.println("Master Request Event");
  if(state == S_HOUR_ANIMATION_HEAD ){
    Wire.write(finishFirstFourRods);
    if(finishFirstFourRods == true){
      state = S_HOUR_ANIMATION_TAIL;
      finishFirstFourRods = false;
    }
  }
}

