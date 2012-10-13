/* ir-distance sketch
* prints distance and changes LED flash rate based on distance from IR sensor
*/
const int ledPin = 13; // the pin connected to the LED to flash
const int sensorPin = 3; // the analog pin connected to the sensor
const long referenceMv = 5000; // long int to prevent overflow when multiplied
void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}
void loop()
{
  int val = analogRead(sensorPin);
  int mV = (val * referenceMv) / 1023;
  Serial.print(mV);
  Serial.print(",");
  int cm = getDistance(mV);
  Serial.println(cm);
  digitalWrite(ledPin, HIGH);
  delay(cm * 10 ); // each centimeter adds 10 milliseconds delay
  digitalWrite(ledPin, LOW);
  delay( cm * 10);
  delay(100);
}

// the following is used to interpolate the distance from a table
// table entries are distances in steps of 250 millivolts
const int TABLE_ENTRIES = 12;
const int firstElement = 250; // first entry is 250 mV
const int INTERVAL = 250; // millivolts between each element
static int distance[TABLE_ENTRIES] = {150,140,130,100,60,50,40,35,30,25,20,15};

int getDistance(int mV)
{
  if( mV > INTERVAL * TABLE_ENTRIES-1 )
      return distance[TABLE_ENTRIES-1];
  else {
    int index = mV / INTERVAL;
    float frac = (mV % 250) / (float)INTERVAL;
    return distance[index] - ((distance[index] - distance[index+1]) * frac);
  }
}
