#include <stdint.h>
#include <ffft.h>
#include <NcrcViz.h>
#include <ledcontroller.h>

#define  IR_AUDIO  0 // ADC channel to capture
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
unsigned long timeWhenHumanVoiceIsDetected;
boolean ledAnimationBegin = false;
unsigned int ledAnimationFrameCounter = 0;
unsigned int ledAnimationNumOfFrames = 30;
unsigned int bgAnimationFrameCounter = 0;
unsigned int bgAnimationNumOfFrames = 100;

// Using Classes from ledcontroller library
using LedController::Color; 
using LedController::LedStrip;
using LedController::ProgressBarSine;
using LedController::RandomMarquee;
using LedController::PatternSineWave;
using LedController::PatternChangingColorColumn;
using LedController::PatternHourGlass;
using LedController::PatternBarPlotToBarPlot;

Color red(0xFF0000);
Color prettyblue(0x6FBAFC);
Color dye(0x6F6F10);
Color purple1(0x800080);
Color purple2(0x700070);
Color purple3(0x600060);
Color purple4(0x500050);

LedStrip ledStrips[] = {LedStrip(PIN_LED1_OUT_SDI, PIN_LED1_OUT_CKI),
                       LedStrip(PIN_LED2_OUT_SDI, PIN_LED2_OUT_CKI),
                       LedStrip(PIN_LED3_OUT_SDI, PIN_LED3_OUT_CKI),
                       LedStrip(PIN_LED4_OUT_SDI, PIN_LED4_OUT_CKI),
                       LedStrip(PIN_LED5_OUT_SDI, PIN_LED5_OUT_CKI),                     
};

// Progress bar
//ProgressBarSine pBarSine = ProgressBarSine(dye, prettyblue);

// RandomMarquee Pattern
//RandomMarquee randomMarquee = RandomMarquee();

// Hour Glass Pattern
PatternHourGlass patternHourGlass = PatternHourGlass(prettyblue, red, purple1);

// Sine Wave Pattern
PatternSineWave patternSineWave = PatternSineWave(red);

// ColorChangingColumn Pattern
PatternChangingColorColumn patternChangingColorColumn = PatternChangingColorColumn(purple1);

// Barplot to Barplot Pattern
PatternBarPlotToBarPlot patternBarPlotToBarPlot = PatternBarPlotToBarPlot(18, 0, prettyblue, red, 1000);

// 
byte token = 0;

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
  
  
  
  // Set the default LED color
//  randomMarquee.update();
//  randomMarquee.apply(ledStrips[1].getColors());
//  ledStrips[1].send();
  
  patternChangingColorColumn.addColor(purple2);
  patternChangingColorColumn.addColor(purple3);
  patternChangingColorColumn.addColor(purple4);

  
  delay(2000);
  
  //establishContact();  // send a byte to establish contact until Processing respon
}

void loop()
{
 
  
  
  if (position == FFT_N)
  {
    


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
    if(detectHumanVoice == true){
      if(humanVoiceHasBeenDetected == true){
          
       
       } else {

          patternHourGlass.update();
          
          humanVoiceHasBeenDetected = true;
          timeWhenHumanVoiceIsDetected = millis();
          if(ledAnimationBegin == false){
            ledAnimationBegin = true;
         }
       }
  
    }
    
    
  
   position = 0;
  }//end position==FFT_N

  //clear background
    for(byte i=0; i < NUM_LED_STRIPS; i++){
      ledStrips[i].clear();
    }


    if(humanVoiceHasBeenDetected == true){
//      randomMarquee.update();
      patternChangingColorColumn.update();
      unsigned long now = millis();
      if(now - timeWhenHumanVoiceIsDetected > 1000){
        humanVoiceHasBeenDetected = false;
      }
    }

//  randomMarquee.apply(ledStrips[1].getColors());
  patternBarPlotToBarPlot.updateSine();
  patternBarPlotToBarPlot.update();
  if(patternBarPlotToBarPlot.isExpired()){
    patternBarPlotToBarPlot = PatternBarPlotToBarPlot(0, 27, prettyblue, purple2, 500);
    token = (++token) % 2;
    if(token == 0){
      patternBarPlotToBarPlot = PatternBarPlotToBarPlot(27, 0, prettyblue, purple2, 500);
    }
    
  }
  switch(token)
  {
     case 0:
       patternBarPlotToBarPlot.apply(ledStrips[0].getColors());
       break;
     case 1:
       patternBarPlotToBarPlot.apply(ledStrips[1].getColors());
       break;
     default:
       patternBarPlotToBarPlot.apply(ledStrips[0].getColors());
       break;
  }
  
  
  patternSineWave.update();
  patternSineWave.apply(ledStrips[2].getColors());
  
  patternChangingColorColumn.apply(ledStrips[3].getColors());
//  
//  patternHourGlass.updateSine();
//  patternHourGlass.apply(ledStrips[1].getColors());
   
  /* Renderer's Code Begin */   
  if(ledAnimationBegin == true){
      
      ledAnimationFrameCounter++;
      if(ledAnimationFrameCounter >= ledAnimationNumOfFrames){
        ledAnimationBegin = false;
        ledAnimationFrameCounter = 0;
      }
      
  }
  
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




