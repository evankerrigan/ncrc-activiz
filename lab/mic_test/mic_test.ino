int yPin = 9;
int rPin = 13;
int micPin = 0;
int micValue = 0;

int lowThresh = 500;
int medThresh = 600;
int highThresh = 1000;


void setup() {
  pinMode(yPin, OUTPUT);
  pinMode(rPin, OUTPUT);  
  //Serial.begin(9600);
}

void loop() {
  int micLevel = analogRead(micPin);
  //Serial.println(micLevel, DEC);     // prints the value read
 /*
  micLevel = map(micLevel, 0, 1023, 0, 255);
  micLevel = constrain(micLevel, 0, 255);
  analogWrite(ledPin, micLevel);
  delay(100);                        // wait 100ms for next reading
  */
  
  if (micLevel > medThresh) {
    digitalWrite(yPin, HIGH);
    digitalWrite(rPin, LOW);    
    delay(10);
    digitalWrite(yPin, LOW);
  } else if (micLevel > lowThresh) {
    digitalWrite(rPin, HIGH);
    digitalWrite(yPin, LOW);
    delay(10);
    digitalWrite(rPin, LOW);
  }
}
