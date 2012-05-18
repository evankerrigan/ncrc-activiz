/**
 * Demo the addressable LED strip from SparkFun (based on the WS2801 IC).
 *
 * This is heavily based on code by Nathan Seidle of SparkFun Electronics,
 * released in 2011 in the public domain (that is, under the Beerware license).
 * The original example [in version history or at
 * http://www.sparkfun.com/datasheets/Components/LED/LED_Strip_Example.pde ]
 * contains more technical details, which here are noted at relevant points in
 * the code.
 *
 * The electrical connections for the strip are:
 *	Power, 5V and Ground (a red/black pair). The listed requirement is 1.8A,
 *		but USB power seems to be sufficient.
 * and data (connect to the end with the arrow pointing into the strip), one of:
 *	4-pin data:
 *		Blue = 5V
 *		Red = SDI (Serial Data Input)
 *		Green = CKI (Clock Input)
 *		Black = GND
 *	5-pin data:
 *		2-pin Red+Black = 5V/GND
 *		Green = CKI
 *		Red = SDI
 */

// #include <newanddelete.h> // For Arduino before 1.0, new and delete operators.
#include <ledcontroller.h>

using LedController::Color;
using LedController::LedStrip;
using LedController::RandomMarquee;

Color orange(0xFF6600);
Color prettyblue(0x6FBAFC);

#define PIN_SDI 2		// Red data wire (not the red 5V wire!)
#define PIN_CKI 3		// Green wire
#define PIN_STATUS_LED 13	// On board LED
#define LED_TRANSITION_TIME 25  // Sets the transition time between individual LEDs

// RandomMarquee marquee = RandomMarquee();
LedStrip ledStrip = LedStrip(PIN_SDI, PIN_CKI);

// int currentLevel = 0;
int backgroundNoise = 0;

int loopCounter = 0;
int currentLED = 0;
int targetLED = 0;

void setup() {
  backgroundNoise = analogRead(A0);

//  pinMode(PIN_STATUS_LED, OUTPUT);
  ledStrip.setup();

  randomSeed(analogRead(0));

  ledStrip.clear();
  ledStrip.send();

  delay(2000);

  for (int i = 0; i < 1000; i++) {
    backgroundNoise = (backgroundNoise + analogRead(A0)) / 2;
  }

  Serial.begin(9600);
}

void loop() {
  int mic;
  if (loopCounter >= LED_TRANSITION_TIME) {
    loopCounter = 0;
    if (targetLED < currentLED) {
      ledStrip.getColors()[currentLED].clear();
      currentLED--;
    }
    else if (targetLED > currentLED) {
      ledStrip.getColors()[currentLED].add(prettyblue.scaled(0.5));
      currentLED++;
    }
    else {
      mic = analogRead(A0);
      mic = removeBackgroundNoise(mic);
      targetLED = micToLED(mic);
      loopCounter = 0;
    }
  }
  
  loopCounter++;
  
  Serial.println(currentLED, DEC);     // prints the current light 
  
  ledStrip.send();
      
      
  /*    
  int mic = analogRead(A0);
  Serial.println(mic, DEC);     // prints the value read
  int prevLevel = currentLevel;
  
  mic = removeBackgroundNoise(mic);
  currentLevel = micToLED(mic);

  Serial.println(currentLevel, DEC);     // prints the current light 

  ledStrip.send();
  */

}


int removeBackgroundNoise(int mic) {
  if (mic < backgroundNoise) {
    mic = 0;
  } 
  else { 
    mic = mic - backgroundNoise;
  }
  return mic;
}


int micToLED(int mic) {
  return map(mic, 0, (1023 - backgroundNoise), 0, 31);
}
