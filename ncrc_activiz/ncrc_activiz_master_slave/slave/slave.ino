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
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  
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
  
  
  // Initialize ADC
  Serial.begin(9600);
  
  
  // Initialize LED strip
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
  ledStrips[i].setup();
  ledStrips[i].clear();
  ledStrips[i].send();
  }
  
  delay(2000);
  
  //establishContact();  // send a byte to establish contact until Processing respon
}

void loop()
{
 
    //draw background for master's LED strips
    for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
      ledStrips[i].clear();
    }
    float scale = 0.45 + 0.35*sin( ( float(bgAnimationFrameCounter)/float(bgAnimationNumOfFrames)) * 2*PI + PI/2 );
    for(byte i=0; i < 32; i++){
      for(byte j=0; j < NUM_LED_STRIPS_SLAVE; j++){
        ledStrips[j].getColors()[i].add(prettyblue.scaled(scale));
      }
    }  
    bgAnimationFrameCounter++;
    if(ledAnimationFrameCounter >= bgAnimationNumOfFrames){
      bgAnimationFrameCounter = 0;
    }
    
    if(humanVoiceHasBeenDetected == true){
        unsigned long now = millis();
        if(now - timeWhenHumanVoiceIsDetected > 3000){
          humanVoiceHasBeenDetected = false;
        }
     } 
   
  /* Renderer's Code Begin */   
  if(ledAnimationBegin == true){
      for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){
        ledStrips[i].clear();
      }
      float scale = 0.45 + 0.35*sin( ( float(ledAnimationFrameCounter)/float(ledAnimationNumOfFrames)) * 2*PI + PI/2 );
      for(byte i=0; i < 32; i++){
        for(byte j=0; j < NUM_LED_STRIPS_SLAVE; j++){
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
  
  for(byte i=0; i < NUM_LED_STRIPS_SLAVE; i++){  
    ledStrips[i].send();
  }
  

}

//I2C Communication, receive event from master
void receiveEvent(int numBytes) //should take a single int parameter (the number of bytes read from master
{
  while (Wire.available()){
    incomingByte = Wire.read();
    if(incomingByte == EVENT_DETECT_HUMAN_VOICE){
      if(humanVoiceHasBeenDetected == true){
  
      } else {
          humanVoiceHasBeenDetected = true;
          timeWhenHumanVoiceIsDetected = millis();
        
          if(ledAnimationBegin == false){
          ledAnimationBegin = true;
        }
      }
      
      
    }
  }
  
}


