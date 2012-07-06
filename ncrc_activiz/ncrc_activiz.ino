#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>
#include <avr/pgmspace.h>

#define IR_AUDIO  0 // ADC channel to capture
#define A_MINUTE 10
#define A_HOUR 10
// Debug
boolean debug = DEBUG_FLAG;
//*****

// For Analog to Digital Converter
volatile  byte  position = 0;
volatile  long  zero = 0;

int16_t capture[FFT_N];			/* Wave captureing buffer */
complex_t bfly_buff[FFT_N];		/* FFT buffer */
uint16_t spektrum[FFT_N/2];		/* Spectrum output buffer */
//*****

//*****
SoundHandler soundHandler = SoundHandler();
boolean humanVoiceHasBeenDetected;

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
                       LedStrip(PIN_LED4_OUT_SDI, PIN_LED4_OUT_CKI)};

// Interval for Controller
Interval oneSec = Interval(1000);
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


// Control flags
bool hourAnimationHasStarted = true;
bool hourAnimationState = FIRST_MOVEMENT_INI;
byte hourAnimationMidMovState = DOWN; //DOWN or UP

// Pattern Sets
PatternChangingColorColumn patCCC = PatternChangingColorColumn(purple1); //Pattern for Led strip 1

PatternHourGlass patHourGlassForSec = PatternHourGlass(prettyblue, oceanicblue, skyblue);
PatternHourGlass patHourGlassForMin = PatternHourGlass(oceanicblue, algaegreen, darkgreen);
PatternHourGlass patHourGlassForPastHour = PatternHourGlass(oceanicblue, algaegreen, darkgreen);

PatternBarPlotToBarPlot patBarPlotForHourAni = PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen, 1000);

void setup()
{  
  pinMode(PIN_IR_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);
  
  // Initialize ADC
  Serial.begin(9600);
  adcInit();
  adcCalb();
  
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS; i++){
    ledStrips[i].setup();
    ledStrips[i].clear();
    ledStrips[i].send();
  }
  
  // Initialize Patterns
  patCCC.addColor(purple2);
  patCCC.addColor(purple3);
  patCCC.addColor(purple3);
  
  
  // Feed fake data for the hour glasses which stored the human voice information in the past hours
  patHourGlassForPastHour.setActualValueBeingStored(5);
  //Serial.println("ProgramStart");
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  delay(1000);
}

void loop()
{
  
  if (position == FFT_N)
  {
    //Serial.print(1);  
    /* Audio Signal Processor Begin*/
    // 1. FFT    
    fft_input(capture, bfly_buff);
    fft_execute(bfly_buff);
    fft_output(bfly_buff, spektrum);
    
    // 2. Audio Recognizer
    boolean detectHumanVoice = soundHandler.containHumanVoice(spektrum, 64);
    if(detectHumanVoice == true){
      /** Set humanVoiceHasBeenDetected to TRUE
       *       ____   ________   <--- humanVoiceHasBeenDetected will be set to TRUE for the while one second period.
       *       |  |   |      |
       * _0sec_|1 |2__|3  4  |5__
      **/
      if(humanVoiceHasBeenDetected == true){
        if(oneSec.update()){
          humanVoiceHasBeenDetected = false;
        }
      } else {
          // First time detect voice within current one second
          humanVoiceHasBeenDetected = true;
          
          /** Should put all the Pattern updates which will only happened *one* time
          *   in each second here.
          *   For example, if you only want to update a Pattern one time when voices is detected in current 
          *   one second, you should put the update code here.
          **/
          patHourGlassForSec.update();
          
          byte timeStoredInHourGlassForSec = patHourGlassForSec.getActualValueBeingStored();
          if(timeStoredInHourGlassForSec == A_MINUTE - 1){
            //patHourGlassForMin.update();
            patHourGlassForSec.restart();
          }
          
      }
      
    }
    
   position = 0;
  }//end position==FFT_N
 
  //*****CONTROLLER & RENDERER BEGIN*************************

  // Every second do ...
  if(oneSec.update()){
    humanVoiceHasBeenDetected = false;
//    currentTimeSec++;
//    if(currentTimeSec >= A_MINUTE){
//      currentTimeSec = 0;
//      currentTimeMin++;
//      if(currentTimeMin >= A_HOUR){
//        hourAnimationHasStarted = true;
//      }
//    }
    oneSec.clearExpired();
  }

  /* Render background */
  // Clear LED strips color
    for(byte i=0; i < NUM_LED_STRIPS; i++){
      ledStrips[i].clear();
    }
    
  // Update the sinosoidal background patterns for all the LED strips inherited PatternSineWave class
    patCCC.updateSine();
    patHourGlassForSec.updateSine();
    patHourGlassForMin.updateSine();
    patHourGlassForPastHour.updateSine();  
  /* finish render background*/

  // 
  if(humanVoiceHasBeenDetected == true){
    // Continusly update within the current one second if human voice has been detected
    // Put all the code that needs to continusly update somethings within the current when human voice
    // is detected to here.
    patCCC.update();
  }
  
  // Hour Animation
//  if(hourAnimationHasStarted){
//    switch(hourAnimationState){
//      case FIRST_MOVEMENT_INI:
//        patBarPlotForHourAni.setStartPosition(patHourGlassesForPastHours[0].getIndicator());
//        patBarPlotForHourAni.setEndPosition(0);
//        patBarPlotForHourAni.setBgColor(patHourGlassesForPastHours[0].getBgColor());
//        patBarPlotForHourAni.setBarColor(patHourGlassesForPastHours[0].getIndicatorColor());
//        hourAnimationState = FIRST_MOVEMENT_UPDATE;
//        break;
//      case FIRST_MOVEMENT_UPDATE:
//        patBarPlotForHourAni.update();
//        if(patBarPlotForHourAni.isExpired()){
//          hourAnimationState = MID_MOVEMENT_INI;
//          
//        }
//        //patBarPlotForHourAni.apply(ledStrips[3].getColors());
//        break;
//      case MID_MOVEMENT_INI:
//        if(true){ // A placeholder here, if the current rod hasn't finished it's animation, keep update it, 
//                  // otherwise, update the second middle rods
//        
//        } else {
//        
//        }
//        
//    }
//    
//  } else {
//    
//  }
  
  // Put all the updated Colors onto the LED strips
  patCCC.apply(ledStrips[0].getColors());
  patHourGlassForSec.apply(ledStrips[1].getColors());
  patHourGlassForMin.apply(ledStrips[2].getColors());
  patHourGlassForPastHour.apply(ledStrips[3].getColors());
  //patHourGlassesForPastHours[1].apply(ledStrips[4].getColors());
  
  for(byte i=0; i < NUM_LED_STRIPS; i++){  
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

// free running ADC fills capture buffer
ISR(ADC_vect)
{
  if (position >= FFT_N)
    return;
  
  capture[position] = ADC + zero;
  if (capture[position] == -1 || capture[position] == 1)
    capture[position] = 0;

  position++;
}
void adcInit(){
  /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
  // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion 
  ADMUX = _BV(REFS0) | IR_AUDIO; // | _BV(ADLAR); 
//  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) //prescaler 64 : 19231 Hz - 300Hz per 64 divisions
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  sei();
}
void adcCalb(){
  //Serial.println("Start to calc zero");
  long midl = 0;
  // get 2 meashurment at 2 sec
  // on ADC input must be NO SIGNAL!!!
  for (byte i = 0; i < 2; i++)
  {
    position = 0;
    delay(100);
    midl += capture[0];
    delay(900);
  }
  zero = -midl/2;
  //Serial.println("Done.");
}
