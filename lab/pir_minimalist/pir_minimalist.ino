int pirPin = 2; //digital 2
int ledPin = 13;

void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
//  digitalWrite(pirPin, HIGH);
  delay(2000);
  Serial.println("Configuration complete. Begin moving!"); 
}

void loop(){
  int pirVal = digitalRead(pirPin);
  digitalWrite(ledPin, LOW);


  if(pirVal == LOW){ //was motion detected
    Serial.println("Motion Detected");
    digitalWrite(ledPin, HIGH);
    delay(2000);
  }

}

