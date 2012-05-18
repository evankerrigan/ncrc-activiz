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

#include <ledcontroller.h>

using LedController::Color;
using LedController::LedStrip;
using LedController::RandomMarquee;

Color orange(0xFF6600);
Color prettyblue(0x6FBAFC);

#define PIN_SDI 2		// Red data wire (not the red 5V wire!)
#define PIN_CKI 3		// Green wire
#define PIN_STATUS_LED 13	// On board LED
#define LED_TRANSITION_TIME 2   // Sets the transition time between individual LEDs
#define NUM_BUCKETS 8         // Sets the number of LED groups on the strip

// RandomMarquee marquee = RandomMarquee();
LedStrip ledStrip = LedStrip(PIN_SDI, PIN_CKI);

// int currentLevel = 0;
int bgNoise = 0;

//int loopCounter = 0;
//int currentLED = 0;
//int targetLED = 0;
int currentBucket = 0;
int prevBucket = 0;

void setup() {
  ledStrip.setup();
  ledStrip.clear();
  ledStrip.send();
  delay(2000);

  Serial.begin(9600);
  Serial.println("Shh!");
  calculateBGNoise();
  delay(1500);
  Serial.println("Start Talking!");
  delay(1500);
}

void loop() {
  int mic = analogRead(A0);

  currentBucket = getMicBucket(mic, bgNoise);
  
//  targetLED = micToLED(removeBGNoise_Munaf(mic));
  
  if (currentBucket > prevBucket) {
    Serial.println("Increasing");     // prints the current light 
    for (int i = prevBucket; i <= currentBucket; i++) {
      setLEDBucket(i);
    }
  }
  else if (currentBucket < prevBucket) {
    Serial.println("Decreasing");     // prints the current light 
    for (int i = currentBucket; i >= prevBucket; i++) {
      clearLEDBucket(i);
    }
  }
  else { Serial.println("I ain't doing shit"); }
    
  prevBucket = currentBucket;

//  String string1 = "Current LED: " + currentLED;
//  String string2 = ", Target LED:" + targetLED;

//  Serial.println(currentLED, DEC);     // prints the current light 

  ledStrip.send();
  
  /*
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
      Serial.println("MIC ME!!!!!!!!!!");
      mic = analogRead(A0);
      mic = removeBGNoise_Munaf(mic);
      targetLED = micToLED(mic);
      loopCounter = 0;
      calculateBGNoise();
    }
  }

  loopCounter++;
*/
}



int getMicBucket(int mic, int background) {
  int micBucket = ((1023 - background) / NUM_BUCKETS);
  mic = (mic);
  return ((mic - background) / micBucket);
}

int setLEDBucket(int bucket) {
  int bucketLength = (32 / NUM_BUCKETS);
  int bucketStart = (bucketLength * bucket);

  for (int i = bucketStart; i <= (bucketStart + bucketLength); i++) {
    ledStrip.getColors()[i].add(prettyblue.scaled(0.5));
  }
}

int clearLEDBucket(int bucket) {
  int bucketLength = (32 / NUM_BUCKETS);
  int bucketStart = (bucketLength * bucket);

  for (int i = bucketStart; i <= (bucketStart + bucketLength); i++) {
    ledStrip.getColors()[i].clear();
  }
}

int calculateBGNoise() {
  bgNoise = analogRead(A0);
  for (int i = 0; i < 500; i++) {
    bgNoise = (bgNoise + analogRead(A0)) / 2;
  }
}

int removeBGNoise(int mic) {
  if (mic < bgNoise) {
    mic = 0;
  } 
  else { 
    mic = mic - bgNoise;
  }
  return map(mic, 0, 1023, 0, (1023-bgNoise));
}

int micToLED(int mic) {
  return map(mic, 0, (1023 - bgNoise), 0, 31);
}