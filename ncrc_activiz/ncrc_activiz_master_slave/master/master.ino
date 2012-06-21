// Chuan-Che Huang & Evan Kerrigan, chuanche@umich.edu
// Purpose: Test 9 LED strips running together, controlled by 2 Arduinos in master-slave mode.
// Title: Sin Wave

#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>
#include <Wire.h>

volatile  byte  position = 0;
volatile  long  zero = 0;

int16_t capture[FFT_N];			/* Wave captureing buffer */
complex_t bfly_buff[FFT_N];		/* FFT buffer */
uint16_t spektrum[FFT_N/2];		/* Spectrum output buffer */

//*****
SoundHandler soundHandler = SoundHandler();
boolean humanVoiceHasBeenDetected;
unsigned long timeWhenHumanVoiceIsDetected;
boolean debug = DEBUG_FLAG;
boolean ledAnimationBegin = false;
unsigned int ledAnimationFrameCounter = 0;
unsigned int ledAnimationNumOfFrames = 30;
unsigned int bgAnimationFrameCounter = 0;
unsigned int bgAnimationNumOfFrames = 100;

//I2C Communication
char incomingByte = 0;

// Using Classes from ledcontroller library
using LedController::Color; 
using LedController::LedStrip;

Color red(0xFFFF00);
Color prettyblue(0x6FBAFC);
Color dye(0x6F6F10);

LedStrip ledStrips[] = {LedStrip(PIN_LED1_OUT_SDI, PIN_LED1_OUT_CKI),
                       LedStrip(PIN_LED2_OUT_SDI, PIN_LED2_OUT_CKI),
                       LedStrip(PIN_LED3_OUT_SDI, PIN_LED3_OUT_CKI),
                       LedStrip(PIN_LED4_OUT_SDI, PIN_LED4_OUT_CKI),
                       LedStrip(PIN_LED5_OUT_SDI, PIN_LED5_OUT_CKI),                     
};

void setup()
{
  //I2C Communication
  Wire.begin();
  
  pinMode(PIN_LED1_OUT_SDI, OUTPUT);
  pinMode(PIN_LED1_OUT_CKI, OUTPUT);
  pinMode(PIN_LED2_OUT_SDI, OUTPUT);
  pinMode(PIN_LED2_OUT_CKI, OUTPUT);
  pinMode(PIN_LED3_OUT_SDI, OUTPUT);
  pinMode(PIN_LED3_OUT_CKI, OUTPUT);
  pinMode(PIN_LED4_OUT_SDI, OUTPUT);
  pinMode(PIN_LED4_OUT_CKI, OUTPUT);
  pinMode(PIN_LED5_OUT_SDI, OUTPUT);
  pinMode(PIN_LED5_OUT_CKI, OUTPUT);
  
  pinMode(PIN_IR_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);
  
  // Initialize ADC
  Serial.begin(9600);
  adcInit();
  adcCalb();
  
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS_MASTER; i++){
  ledStrips[i].setup();
  ledStrips[i].clear();
  ledStrips[i].send();
  }
  
  delay(2000);
  
  //establishContact();  // send a byte to establish contact until Processing respon
}

void loop()
{
 
  
  
  if (position == FFT_N)
  {
    
    //draw background for master's LED strips
    for(byte i=0; i < NUM_LED_STRIPS_MASTER; i++){
      ledStrips[i].clear();
    }
    float scale = 0.45 + 0.35*sin( ( float(bgAnimationFrameCounter)/float(bgAnimationNumOfFrames)) * 2*PI + PI/2 );
    for(byte i=0; i < 32; i++){
      for(byte j=0; j < NUM_LED_STRIPS_MASTER; j++){
        ledStrips[j].getColors()[i].add(prettyblue.scaled(scale));
      }
    }  
    bgAnimationFrameCounter++;
    if(ledAnimationFrameCounter >= bgAnimationNumOfFrames){
      bgAnimationFrameCounter = 0;
    }
    
    /* Audio Signal Processor Begin*/
    // 1. FFT    
    fft_input(capture, bfly_buff);
    fft_execute(bfly_buff);
    fft_output(bfly_buff, spektrum);

//    debug && Serial.println("Raw spectrum");
//    for(byte i = 0; i < 20; i++){
//      Serial.println(spektrum[i]);
//    }
    
//    Serial.println("--end--");
    
    // 2. Audio Recognizer
    boolean detectHumanVoice = soundHandler.containHumanVoice(spektrum, 64);
    
    
    // 3. State Machine Code, Also a Central Controller 
    // Controller should set different rendering flag in order to run different 
    // led animations.
    if(detectHumanVoice == true){
//      debug && Serial.println("**********************HUMAN VOICE***************");
      if(humanVoiceHasBeenDetected == true){
        //...
      } else {
        // Send signal to slave to activate the LED
        sendEventToSlave(EVENT_DETECT_HUMAN_VOICE);
        
        // Change the control flag and begin render the special animation
        humanVoiceHasBeenDetected = true;
        timeWhenHumanVoiceIsDetected = millis();
        
        if(ledAnimationBegin == false){
          ledAnimationBegin = true;
        }
      }
  
    }
    if(humanVoiceHasBeenDetected == true){
      unsigned long now = millis();
      if(now - timeWhenHumanVoiceIsDetected > 3000){
        humanVoiceHasBeenDetected = false;
      }
    } 
   
   
  /* Renderer's Code Begin */   
  if(ledAnimationBegin == true){
      for(byte i=0; i < NUM_LED_STRIPS_MASTER; i++){
        ledStrips[i].clear();
      }
      float scale = 0.45 + 0.35*sin( ( float(ledAnimationFrameCounter)/float(ledAnimationNumOfFrames)) * 2*PI + PI/2 );
      for(byte i=0; i < 32; i++){
        for(byte j=0; j < NUM_LED_STRIPS_MASTER; j++){
          if(i % 5 == 0){
            ledStrips[j].getColors()[i].setCombinedValue(0x106F10);
          } else { 
            ledStrips[j].getColors()[i].add(red.scaled(scale));
          }
        }
      }
      
      ledAnimationFrameCounter++;
      if(ledAnimationFrameCounter >= ledAnimationNumOfFrames){
        ledAnimationBegin = false;
        ledAnimationFrameCounter = 0;
      }
      
  }
  
  for(byte i=0; i < NUM_LED_STRIPS_MASTER; i++){  
    ledStrips[i].send();
  }
  
   position = 0;
  }//end position==FFT_N

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
  Serial.println("Start to calc zero");
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
  Serial.println("Done.");
}

//I2C Communication
void sendEventToSlave(byte event)
{
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(event);
  Wire.endTransmission();
}

