#include <ledcontroller.h>

#include <NcrcViz.h> 
#include <stdint.h>
#include <ffft.h>





//************************************************************************************
//  Level Management
//  
//  A level is a group of LEDs that correspond to a specific microphone amplitude.
//  Higher microphone amplitude = greater level.
//  
//  Levels start at zero. The number of levels per LED strip can be configured below.
//************************************************************************************

#define NUM_LEVELS 32
#define MAX_LED_LEVEL (NUM_LEVELS - 1)
#define NUM_LEDS_LEVEL (NUM_LEDS / NUM_LEVELS)

//*******
SoundHandler soundHandler = SoundHandler(); 
boolean debug = DEBUG_FLAG;
boolean raw = RAW_DATA_FLAG;
// loop's variables

boolean humanVoiceHasBeenDetected;
unsigned long timeWhenHumanVoiceIsDetected;
boolean ledAnimationBegin = false;
unsigned int ledAnimationFrameCounter = 0;
unsigned int ledAnimationNumOfFrames = 100;

// global variable for ADC
volatile byte position = 0;
volatile long zero = 0;

int16_t capture[FFT_N];
complex_t bfly_buff[FFT_N];
uint16_t spectrum[FFT_N/2];

void setup(){
  // 5 LED strip's pins
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
  
  // IR and Mic's pins
  pinMode(PIN_IR_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);

  // Serial Input for debug use
  Serial.begin(9600);
  
  // Initialize ADC
  adcInit();
  adcCalb();
  Serial.println("Program Start");
  
  //clear the LED strip
//  for(int i=0; i < NUM_LED_STRIPS; i++){
//    ledStrips[i].setup();
//    ledStrips[i].clear();
//    ledStrips[i].send();
//  }
  
  
  //Wait for 2 seconds to start the program
  establishContact();
  
}

void loop(){
  // Draw background
  
  
  if(position == FFT_N){
    fft_input(capture, bfly_buff);
    fft_execute(bfly_buff);
    fft_output(bfly_buff, spectrum);
    
    Serial.println("Raw spectrum");
    for(byte i = 0; i < 20; i++){
      Serial.println(spectrum[i]);
    }
    
    Serial.println("--end--");
    boolean detectHumanVoice = soundHandler.containHumanVoice(spectrum, 64);
    
    if(detectHumanVoice == true){
      if(humanVoiceHasBeenDetected == true){
        // ...
      } else {
        debug && Serial.println("Detect Human Voice");
        humanVoiceHasBeenDetected = true;
        timeWhenHumanVoiceIsDetected = millis();
        if(ledAnimationBegin == false){
          ledAnimationBegin = true;
        }
      }
    }
    
    Serial.println("--1--");
    
  
    if(humanVoiceHasBeenDetected == true){
      //every detect will exist for 3 seconds
      unsigned long now = millis();
      if( now - timeWhenHumanVoiceIsDetected > 3000 ){ //over 3 seconds
        humanVoiceHasBeenDetected = false;
      } 
    }
    
    Serial.println("--2--");
    
    // Draw foreground
    if(ledAnimationBegin == true){
      debug && Serial.println(ledAnimationFrameCounter);
      
      ledAnimationFrameCounter++;
      if(ledAnimationFrameCounter >= ledAnimationNumOfFrames){
        ledAnimationBegin = false;
        ledAnimationFrameCounter = 0;
      }
    }
    
    Serial.println("--3--");
    
    // Should reset position at the end of the if statement!!!

  }
}

/*
*  Analog to Digital Converter Management
*
*/

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
  ADMUX = _BV(REFS0) | PIN_MIC_IN; // | _BV(ADLAR); 
//  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) //prescaler 64 : 19231 Hz - 300Hz per 64 divisions
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  sei(); //Enables interrupts by setting the global interrupt mask. 
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

void establishContact() {
 while (Serial.available() <= 0) {
      Serial.write('A');   // send a capital A
      delay(300);
  }
}



