// 1. Reads mic input
// 2. Maps mic level input to LED output values
// 3. Generates "equalizer visualization using 4 LEDs based on mic level input



int ledPin[] = {
  2, 4, 7, 8};
int currentLevel = 0;

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  int mic = analogRead(A0);
  Serial.println(mic, DEC);     // prints the value read
  int prevLevel = currentLevel;

  if (mic < 500) {
    mic = 0;
  } 
  else { 
    mic = mic - 500;
  }


  currentLevel = map(mic, 0, 523, 0, 3);

  if (prevLevel > currentLevel) {
    for (int i = prevLevel; i >= currentLevel; i--) {
      digitalWrite(ledPin[i], LOW);
    }
  } 
  else if (currentLevel > prevLevel) {
    for (int i = prevLevel; i <= currentLevel; i++) {
      digitalWrite(ledPin[i], HIGH);
    }
  }
}
