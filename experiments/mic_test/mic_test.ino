int val;

int audioPort = 0;
int yellowLed = 4;
int redLed = 7;

int lowThreshold = 780;
int mediumThreshold = 600;
int highThreshold = 900;

void setup() {
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);  
  Serial.begin(9600);
}

void loop() {
  val = analogRead(audioPort);
  Serial.println(val, DEC);     // prints the value read
  if(val > highThreshold) { 
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, HIGH);
    delay(10);              
    digitalWrite(redLed, LOW); 
  } 
  else if(val > mediumThreshold) {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    delay(10);              
    digitalWrite(yellowLed, LOW); 
  }
}


