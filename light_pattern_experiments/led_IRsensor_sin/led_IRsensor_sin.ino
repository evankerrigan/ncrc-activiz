#include <ledcontroller.h>

using LedController::Color;
using LedController::LedStrip;
using LedController::RandomMarquee;

Color orange(0xFF6600);
Color prettyblue(0x6FBAFC);

#define PIN_SDI 4		// Red data wire (not the red 5V wire!)
#define PIN_CKI 5		// Green wire
#define PIN_SDI_2 2		// Red data wire (not the red 5V wire!)
#define PIN_CKI_2 3		// Green wire
#define PIN_MIC A1              // Microphone input
#define PIN_IN_IR 13

#define NUM_LEDS 32

// RandomMarquee marquee = RandomMarquee();
LedStrip ledStrip = LedStrip(PIN_SDI, PIN_CKI);
LedStrip ledStrip2 = LedStrip(PIN_SDI_2, PIN_CKI_2);

// Global environment variables.
int rawMicValue;
int rawBackgroundNoise;
byte currentLevel = 0;
int testCounter;
float wavePeriod = 2*PI;
int currentSampleIndex = 0;
const int defaultNumOfSamples = 5000;
const int motionDetectedNumOfSamples = 512;
int currentNumOfSamples = defaultNumOfSamples;
// Sin wave
// nrightnessScale = 0.5 + 0.5*sin( (2pi/wavePeriod) * t)
int ledPatternTimeAfterMotionIsDetected = 2000;
int counterForLedPatternTime = 0;
int motionHasBeenDetected = 0; // 0: Nope, 1: Somebody had moved before!

long debounceDelay = 50;
long lastDebounceTime = 0;
int motionState;
int lastMotionState;

void setup() {
  // declare LED as output
  pinMode(PIN_SDI, OUTPUT);
  pinMode(PIN_CKI, OUTPUT);
  pinMode(PIN_SDI_2, OUTPUT);
  pinMode(PIN_CKI_2, OUTPUT);
  pinMode(PIN_IN_IR, INPUT);
  
  // Gather Background Noise
  Serial.begin(9600);
  Serial.println("Calculating background noise.");
  calculateBGNoise();
  testCounter = 0;
  
  // Clear the LED Strip
  ledStrip.setup();
  ledStrip2.setup();
  ledStrip.clear();
  ledStrip2.clear();  
  ledStrip.send();
  ledStrip2.send();
  
  delay(2000);
  
  Serial.println("Start Talking!");
}

void loop() {
  int tempLevel;
  int motionIsDetected = digitalRead(PIN_IN_IR);
  
  Serial.println(motionIsDetected);
  if(motionHasBeenDetected == 1){ // motion had been detected n loops before
    // count down the
    Serial.println("Motion Has Been Detected.");
    counterForLedPatternTime++;    
    if(counterForLedPatternTime >= ledPatternTimeAfterMotionIsDetected){
      counterForLedPatternTime = 0;
      motionHasBeenDetected = 0;
      currentNumOfSamples = defaultNumOfSamples;
    }
  } else {
    if(motionIsDetected == HIGH){
      Serial.println("Detect Motion");
      motionHasBeenDetected == 1;
      currentNumOfSamples = motionDetectedNumOfSamples;
    }
  
  }
  for(int i = 0; i < NUM_LEDS ; i++){
    clearLevel(i);
    setLevel(i);
  }
  ledStrip.send();
  ledStrip2.send();  
}


//************************************************************************************
// Sin Wave Patterns



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

byte levelLEDStart(byte level) {
  // Levels start at 0
  // # Pins / # Levels * Level = start
  return constrain((NUM_LEDS / NUM_LEVELS) * level, 0, (NUM_LEDS - NUM_LEDS_LEVEL - 1));
}

byte levelLEDEnd(byte level) {
  // Levels start at 0
  // # Pins / # Levels * Level + Pins-per-level = end
    
  return constrain(levelLEDStart(level) + (NUM_LEDS_LEVEL-1), 0, NUM_LEDS - 1);
}

// Clears a level (group of LEDs). Does not actually write to LED strip!
void clearLevel(byte level) {
  // from start to end of level, run ledstrip.clear()
  
  byte first_led = levelLEDStart(level);
  byte last_led = levelLEDEnd(level);
    
  for (byte i = first_led; i <= last_led; i++) {
    ledStrip.getColors()[i].clear();
    ledStrip2.getColors()[i].clear();
  }
}

// Colorizes a level (group of LEDs). Does not actually write to LED strip!
void setLevel(byte level) {
  // from start to end of level, run ledstrip.add()
  int first_led = levelLEDStart(level);
  int last_led = levelLEDEnd(level);
  float scale;
  float scale2;
  
  scale = 0.55 + 0.45*sin( ( float(currentSampleIndex)*wavePeriod/float(currentNumOfSamples)));
  scale2 =  0.55 + 0.45*sin( ( float(currentSampleIndex)*wavePeriod/float(currentNumOfSamples)) + PI );
  //scale += 0.4 + 0.3*cos( ( float(currentSampleIndex)*0.5*wavePeriod/float(numOfSamples)) + PI/2.3);
  for (int i = first_led; i <= last_led; i++) {
    ledStrip.getColors()[i].add(prettyblue.scaled(scale));
    ledStrip2.getColors()[i].add(prettyblue.scaled(scale2)); 
  }
  
  currentSampleIndex++;
  if(currentSampleIndex >= currentNumOfSamples ){
    currentSampleIndex = 0;
    //numOfSamples = random(300, 10000);
  } 
}

// Changes the mic amplitude range based on background noise.
// It's not perfect due to the crappy background noise detection method.
int filterRawMic() {
  return constrain(rawMicValue - rawBackgroundNoise, 0, 1023 - rawBackgroundNoise);
}

// Given an LED number, return its level
byte LEDToLevel(byte led) {
  return constrain((led/NUM_LEDS_LEVEL), 0, MAX_LED_LEVEL);
}

// Converts raw mic value to level using LEDToLevel helper function
byte convertMicToLevel() {
  int LED = map(filterRawMic(), 0, 1023-rawBackgroundNoise, 0, NUM_LEDS - 1);
  
  return LEDToLevel(LED);
}

// Admittedly stupid way of calculating background noise.
// Consider this temporary and use proper signal processing techniques later.
void calculateBGNoise() {
  unsigned long noiseSum = 0;
  int numReadings = 1000;
  
  for (unsigned long i = 0; i < numReadings; i++) {
    unsigned long tempPinMic = analogRead(PIN_MIC);
    noiseSum += tempPinMic;
  }
  
  rawBackgroundNoise = noiseSum / numReadings;
}


