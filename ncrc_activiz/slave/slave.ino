#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>
#include <avr/pgmspace.h>

#define A_MINUTE 10
#define A_HOUR 10
// Debug
boolean debug = DEBUG_FLAG;
//*****

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
bool hourAnimationHasStarted = true;  // Master --> Slave
byte hourAnimationState = FIRST_MOVEMENT_INI;
byte hourAnimationToken = NUM_LED_STRIPS_SLAVE -1; //The Token starts from the last led strip
byte hourAnimationMidMovState = DOWN; //DOWN or UP
byte ledStripsState[NUM_LED_STRIPS_SLAVE];
byte tempValueForHourGlass;
// Pattern Sets

PatternHourGlass patHourGlassesForPastHours[] = {PatternHourGlass(oceanicblue, algaegreen, darkgreen),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen),
                                                 PatternHourGlass(oceanicblue, algaegreen, darkgreen)};

PatternHourGlass tempHourGlass = PatternHourGlass(oceanicblue, algaegreen, darkgreen);

PatternBarPlotToBarPlot patBarPlotsForHourAni[] = {PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen, 1000),
                                                  PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen, 1000),
                                                  PatternBarPlotToBarPlot(0, 30, oceanicblue, algaegreen, 1000)};

void setup()
{  
  // Initialize ADC
  Serial.begin(9600);
  
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    ledStrips[i].setup();
    ledStrips[i].clear();
    ledStrips[i].send();
  }
  
  // Feed fake data for the hour glasses which stored the human voice information in the past hours
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
    patHourGlassesForPastHours[i].setActualValueBeingStored(5);
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
  
  //*****CONTROLLER & RENDERER BEGIN*************************


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
        debug && Serial.println("FIRST_MOVEMENT_INI");
        patBarPlotsForHourAni[DOWN].setStartPosition(patHourGlassesForPastHours[hourAnimationToken].getIndicator());
        patBarPlotsForHourAni[DOWN].setEndPosition(0);
        patBarPlotsForHourAni[DOWN].setBgColor(patHourGlassesForPastHours[hourAnimationToken].getBgColor());
        patBarPlotsForHourAni[DOWN].setBarColor(patHourGlassesForPastHours[hourAnimationToken].getIndicatorColor());
        ledStripsState[hourAnimationToken] = HOUR_ANIMATION_DOWN;
        hourAnimationState = FIRST_MOVEMENT_UPDATE;
        break;
      case FIRST_MOVEMENT_UPDATE:  //DOWN
        debug && Serial.println("FIRST_MOVEMENT_UPDATE");
        patBarPlotsForHourAni[DOWN].update();
        debug && Serial.print("isExpired=");
        debug && Serial.println(patBarPlotsForHourAni[0].isExpired());
        if(patBarPlotsForHourAni[DOWN].isExpired()){
        
          ledStripsState[hourAnimationToken] = REMAIN;
          hourAnimationState = MID_MOVEMENT_INI;
          hourAnimationToken--;
        }
        //patBarPlotForHourAni.apply(ledStrips[3].getColors());
        break;
      case MID_MOVEMENT_INI:
        if(hourAnimationMidMovState == DOWN){ // Leds go down
          debug && Serial.println("MID_MOVEMENT_INI DOWN");
          tempValueForHourGlass = patHourGlassesForPastHours[hourAnimationToken].getActualValueBeingStored();
          ledStripsState[hourAnimationToken] = HOUR_ANIMATION_DOWN;
          patBarPlotsForHourAni[DOWN].restart();
          patBarPlotsForHourAni[DOWN].setStartPosition(patHourGlassesForPastHours[hourAnimationToken].getIndicator());
          patBarPlotsForHourAni[DOWN].setEndPosition(0);
          patBarPlotsForHourAni[DOWN].setBgColor(patHourGlassesForPastHours[hourAnimationToken].getBgColor());
          patBarPlotsForHourAni[DOWN].setBarColor(patHourGlassesForPastHours[hourAnimationToken].getIndicatorColor());
          
          hourAnimationState = MID_MOVEMENT_UPDATE;
        } else { // LEDS go up
          debug && Serial.println("MID_MOVEMENT_INI UP");
          ledStripsState[hourAnimationToken] = HOUR_ANIMATION_UP;
          patBarPlotsForHourAni[UP].restart();
          patBarPlotsForHourAni[UP].setStartPosition(0);
          patBarPlotsForHourAni[UP].setEndPosition(tempValueForHourGlass);
          patBarPlotsForHourAni[UP].setBgColor(patHourGlassesForPastHours[hourAnimationToken].getBgColor());
          patBarPlotsForHourAni[UP].setBarColor(patHourGlassesForPastHours[hourAnimationToken].getIndicatorColor());
          
          hourAnimationState = MID_MOVEMENT_UPDATE;
        }
        break;
      case MID_MOVEMENT_UPDATE:
        debug && Serial.println("MID_MOVEMENT_UPDATE");
        bool isExpired;
        if(hourAnimationMidMovState == DOWN){
          patBarPlotsForHourAni[DOWN].update();
          isExpired = patBarPlotsForHourAni[DOWN].isExpired();
        }
        else {
          patBarPlotsForHourAni[UP].update();
          isExpired = patBarPlotsForHourAni[UP].isExpired();
        }
        
        if(isExpired){
          if(/*NEED TO MOVE TO NEXT ROD && NEXT ROD = LAST ONE*/ hourAnimationMidMovState == UP && hourAnimationToken ==  1)
          {
            debug && Serial.println("GO LAST ROUND");
            
          } else if(/*FROM DOWN TO UP*/ hourAnimationMidMovState == DOWN){
            debug && Serial.println("GO FROM DOWN TO UP");
            
            //State Transition Tasks
            ledStripsState[hourAnimationToken] = HOUR_ANIMATION_REMAIN;
            hourAnimationState = MID_MOVEMENT_INI;
            hourAnimationMidMovState = UP;
            hourAnimationToken++;
            
          } else if(/*NEED TO MOVE TO NEXT ROD*/ hourAnimationMidMovState == UP){
              debug && Serial.println("GO NEXT ROUND");
            
            
              // State Transition Task
              hourAnimationState = MID_MOVEMENT_INI;
              hourAnimationMidMovState = DOWN;
              // Change the current rod back to hourglass
              ledStripsState[hourAnimationToken] = NORMAL;
              
              patHourGlassesForPastHours[hourAnimationToken].setActualValueBeingStored(tempValueForHourGlass);
              hourAnimationToken -= 2;
          }
          
        }
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


/* Facility Functions Begin */
void establishContact() {
 while (Serial.available() <= 0) {
      Serial.write('A');   // send a capital A
      delay(300);
  }
}

