#include <ledcontroller.h>

using LedController::Color;
using LedController::LedStrip;
using LedController::RandomMarquee;

Color orange(0xFF6600);
Color prettyblue(0x6FBAFC);

#define PIN_SDI 2		// Red data wire (not the red 5V wire!)
#define PIN_CKI 3		// Green wire
#define PIN_MIC A0              // Microphone input

// RandomMarquee marquee = RandomMarquee();
LedStrip ledStrip = LedStrip(PIN_SDI, PIN_CKI);

// Global environment variables.
int rawMicValue;
int rawBackgroundNoise;
byte currentLevel = 0;

void setup() {
  // Gather Background Noise
  Serial.begin(9600);
  Serial.println("Calculating background noise.");
  calculateBGNoise();
  
  // Clear the LED Strip
  ledStrip.setup();
  ledStrip.clear();
  ledStrip.send();
  
  delay(2000);
  
  Serial.println("Start Talking!");
}

void loop() {
  int tempLevel;
  
  // Read raw mic value
  rawMicValue = analogRead(PIN_MIC);

  // Filter mic value then convert to level
  tempLevel = convertMicToLevel();
  
  // Clear or Set levels based on current filtered mic value
  if (tempLevel < currentLevel) {
    Serial.println("SHOULD BE CLEARLING LEVEL");
    for (byte i = currentLevel; i >= tempLevel; i--) {
      clearLevel(i);
    }
    ledStrip.send();
  }
  else if (tempLevel > currentLevel) {
    Serial.println("SHOULD BE SETTING LEVEL");
    for (byte i = currentLevel; i <= tempLevel; i++) {
      setLevel(i);
    }
    ledStrip.send();
  }
  
  currentLevel = tempLevel;
}


//************************************************************************************
//  Level Management
//  
//  A level is a group of LEDs that correspond to a specific microphone amplitude.
//  Higher microphone amplitude = greater level.
//  
//  Levels start at zero. The number of levels per LED strip can be configured below.
//************************************************************************************
#define NUM_LEDS 32
#define NUM_LEVELS 8
#define MAX_LED_LEVEL (NUM_LEVELS-1)
#define NUM_LEDS_LEVEL 4

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
  }
}

// Colorizes a level (group of LEDs). Does not actually write to LED strip!
void setLevel(byte level) {
  // from start to end of level, run ledstrip.add()
  int first_led = levelLEDStart(level);
  int last_led = levelLEDEnd(level);
  
  Serial.println("first led");
  Serial.println(first_led);
  Serial.println("last_led");
  Serial.println(last_led);
  for (int i = first_led; i <= last_led; i++) {
    ledStrip.getColors()[i].add(prettyblue.scaled(0.5));
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
  int noiseSum = 0;
  int numReadings = 1000;
  
  for (int i = 0; i < numReadings; i++) {
    noiseSum += analogRead(PIN_MIC);
  }
  
  rawBackgroundNoise = noiseSum / numReadings;
}


