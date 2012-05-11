int lightPin = 0;
int ledPin = 9;

void setup () {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int lightLevel = analogRead(lightPin);
  lightLevel = map(lightLevel, 0, 900, 0, 120);
  lightLevel = constrain(lightLevel, 0, 120);
  analogWrite(ledPin, lightLevel);
}

