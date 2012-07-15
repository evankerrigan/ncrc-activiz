#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>
//#include <avr/pgmspace.h>
#include <Wire.h>

#define IR_AUDIO  0 // ADC channel to capture
#define A_MINUTE 60
#define A_HOUR 60
// Debug
boolean debug = DEBUG_FLAG;
//*****

// Master and Slave protocal
#define EVENT_HA_START 0  //HA = Hour Animation
#define EVENT_HA_LAST_ROD 1

#define SLAVE_ADDRESS 1

#define S_NORMAL 0
#define S_WAITING_RESPONSE 1
#define S_HOUR_ANIMATION 2

byte state = S_NORMAL;

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

// Interval for Controller
Interval oneSec = Interval(1000);
byte currentTimeSec = 0;
byte currentTimeMin = 0;

// Hour Animation States
#define S00 0
#define S01 1
#define S10 2
#define S11 3
#define S20 4
#define S21 5

// LED Light Mode
#define NORMAL 0
#define HOUR_ANIMATION_UP 1
#define HOUR_ANIMATION_DOWN 2
#define HOUR_ANIMATION_REMAIN 3

// Hour Animation Internal States for Middle Rods
#define DOWN 0
#define UP 1


// Hour Animation Variables
byte tempValue;
byte tempIndicator;
Color tempBgColor;
Color tempBarColor;
bool hourAnimationHasStarted = false;
bool hourAnimationForMasterHasStarted = false;
byte hourAnimationState = S00;
byte statePatHourGlassForPastHour = NORMAL;
byte statePatHourGlassForMin = NORMAL;

byte second = 0;
byte minute = 0;



// Pattern Sets
PatternChangingColorColumn patCCC = PatternChangingColorColumn(purple1); //Pattern for Led strip 1

PatternHourGlass patHourGlassForSec = PatternHourGlass(oceanicblue, algaegreen, darkgreen);
PatternHourGlass patHourGlassForMin = PatternHourGlass(oceanicblue, algaegreen, darkgreen);
PatternHourGlass patHourGlassForPastHour = PatternHourGlass(oceanicblue, algaegreen, darkgreen, false);
//
PatternBarPlotToBarPlot patBarPlotForHourAni = PatternBarPlotToBarPlot(30, 0, oceanicblue, algaegreen);
PatternBarPlotToBarPlot patBarPlotForHourAniRemain = PatternBarPlotToBarPlot(0, 0, oceanicblue, oceanicblue);

void setup()
{  
  pinMode(PIN_IR_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);
  
  // Initialize ADC
  Serial.begin(9600);
  adcInit();
  adcCalb();
  
  Wire.begin();  // Join I2C bus as a master
  
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
  patHourGlassForPastHour.setActualValueBeingStored(29);
  patHourGlassForMin.setActualValueBeingStored(15);
  
  // Reset Barplot remain
  patBarPlotForHourAniRemain.setStartPosition(0);
  
  //Serial.println("ProgramStart");
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  delay(1000);
}

void loop()
{
  Serial.print("state=");
  Serial.println(state);
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
          
          // I2C Testing
          
          sendEvent(10);  //test
          
          /** Should put all the Pattern updates which will only happened *one* time
          *   in each second here.
          *   For example, if you only want to update a Pattern one time when voices is detected in current 
          *   one second, you should put the update code here.
          **/
          patHourGlassForSec.update();
          second++;
          
          ;
          if(second == A_MINUTE){
            patHourGlassForMin.update();
            //patHourGlassForSec.restart();
            second =0;
            minute++;
            if(minute == A_HOUR){
              sendEvent(patHorGlassForMin.getActualValueBeingStored());
            }
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
    patBarPlotForHourAni.updateSine();
    patBarPlotForHourAniRemain.updateSine();
  /* finish render background*/

  // 
  if(humanVoiceHasBeenDetected == true){
    // Continusly update within the current one second if human voice has been detected
    // Put all the code that needs to continusly update somethings within the current when human voice
    // is detected to here.
    patCCC.update();
  }
  // Need to hook this control flag with master's state changing
  if(hourAnimationForMasterHasStarted){
    Serial.print(" S=");
    Serial.println(hourAnimationState);
    switch(hourAnimationState){
      case S00:  // Initilize ROD 3
        patBarPlotForHourAni.setStartPosition( patHourGlassForPastHour.getIndicator() );
        patBarPlotForHourAni.setEndPosition(0);
        patBarPlotForHourAni.setBgColor( patHourGlassForPastHour.getBgColor() );
        patBarPlotForHourAni.setBarColor( patHourGlassForPastHour.getIndicatorColor() );
        statePatHourGlassForPastHour = HOUR_ANIMATION_DOWN;
        hourAnimationState = S01;
        break;
      case S01:  // ROD 3's LED GO DOWN
        patBarPlotForHourAni.update();
        if(patBarPlotForHourAni.isExpired()){
          
          statePatHourGlassForPastHour = HOUR_ANIMATION_REMAIN;
          hourAnimationState = S10;
        }  
        break;
      case S10:  // Initialize ROD 2
        tempValue = patHourGlassForMin.getActualValueBeingStored();
        tempIndicator = patHourGlassForMin.getIndicator();
        tempBgColor = patHourGlassForMin.getBgColor();
        tempBarColor = patHourGlassForMin.getIndicatorColor();  
        patBarPlotForHourAni.setStartPosition(tempIndicator);
        patBarPlotForHourAni.setEndPosition(0);
        patBarPlotForHourAni.setBgColor(tempBgColor);
        patBarPlotForHourAni.setBarColor(tempBarColor);
        patBarPlotForHourAni.restart();
        statePatHourGlassForMin = HOUR_ANIMATION_DOWN;
        hourAnimationState = S11;
        break;
      case S11:  // ROD 2's LED GO DOWN
        patBarPlotForHourAni.update();
        if(patBarPlotForHourAni.isExpired()){
          statePatHourGlassForMin = HOUR_ANIMATION_REMAIN;
          hourAnimationState = S20;
        }
        break;
      case S20: //Initialize ROD 3, prepare to go Up
        statePatHourGlassForPastHour = HOUR_ANIMATION_UP;
        patBarPlotForHourAni.setStartPosition(0);
        patBarPlotForHourAni.setEndPosition(tempIndicator);
        patBarPlotForHourAni.setBgColor(tempBgColor);
        patBarPlotForHourAni.setBarColor(tempBarColor);
        patBarPlotForHourAni.restart();
        hourAnimationState = S21;
        break;
      case S21:
        patBarPlotForHourAni.update();
        if(patBarPlotForHourAni.isExpired()){
          
          patHourGlassForPastHour.setActualValueBeingStored(tempValue);
          patHourGlassForMin.setActualValueBeingStored(0);
          statePatHourGlassForPastHour = NORMAL;
          statePatHourGlassForMin = NORMAL;
          hourAnimationForMasterHasStarted = false;
          hourAnimationState = S00;
        }
        break;
    }
  }
  // Put all the updated Colors onto the LED strips
  // Rod 0
  patCCC.apply(ledStrips[0].getColors());
  
  // Rod 1
  patHourGlassForSec.apply(ledStrips[1].getColors());
  
  // Rod 2
  switch(statePatHourGlassForMin)
  {
    case NORMAL:
      patHourGlassForMin.apply(ledStrips[2].getColors());
      break;
    case HOUR_ANIMATION_UP:
      patBarPlotForHourAni.apply(ledStrips[2].getColors());
      break;
    case HOUR_ANIMATION_DOWN:
      patBarPlotForHourAni.apply(ledStrips[2].getColors());
      break;
    case HOUR_ANIMATION_REMAIN:
      patBarPlotForHourAniRemain.apply(ledStrips[2].getColors());
      break;
    default:
      patHourGlassForMin.apply(ledStrips[2].getColors());
      break;
  }
  
  // Rod 3
  Serial.print(" R3=");
  Serial.println(statePatHourGlassForPastHour);
  switch(statePatHourGlassForPastHour)
  {
    case NORMAL:
      patHourGlassForPastHour.apply(ledStrips[3].getColors());
      break;
    case HOUR_ANIMATION_UP:
      patBarPlotForHourAni.apply(ledStrips[3].getColors());
      break;
    case HOUR_ANIMATION_DOWN:
      patBarPlotForHourAni.apply(ledStrips[3].getColors());
      break;
    case HOUR_ANIMATION_REMAIN:
      patBarPlotForHourAniRemain.apply(ledStrips[3].getColors());
      break;
    default:
      patHourGlassForPastHour.apply(ledStrips[3].getColors());
      break;
  }
  
  for(byte i=0; i < NUM_LED_STRIPS; i++){  
    ledStrips[i].send();
  }
  
  // I2C Communication
  if( state == S_WAITING_RESPONSE){
    byte slaveFinished = requestSlaveState();
    Serial.print("debug,slaveFinished=");
    Serial.println(slaveFinished);
    if(slaveFinished){
      state = S_HOUR_ANIMATION;
      hourAnimationForMasterHasStarted = true;
    }
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
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1); //prescaler 64 : 19231 Hz - 300Hz per 64 divisions
  //ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
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

void sendEvent(byte value)
{
  
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(value);
  Wire.endTransmission();
  state = S_WAITING_RESPONSE;
}

byte requestSlaveState()
{
  debug && Serial.println("requestSlaveState");
  byte slaveFinished = 0;
  Wire.requestFrom(SLAVE_ADDRESS,1);
  if(Wire.available()){
    slaveFinished = Wire.read();
    Serial.print("slaveFinished()=");
    Serial.println(slaveFinished);
  }
  return slaveFinished;
}
