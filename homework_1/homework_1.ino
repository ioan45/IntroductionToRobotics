const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int redPotPin = A0;
const int greenPotPin = A1;
const int bluePotPin = A2;

const int minPotValue = 0;
const int maxPotValue = 1023;
const int minLedValue = 0;
const int maxLedValue = 255;

void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  
  pinMode(redPotPin, INPUT);
  pinMode(greenPotPin, INPUT);
  pinMode(bluePotPin, INPUT);
}

void loop() {
  adjustLedValue(redPotPin, redLedPin);
  adjustLedValue(greenPotPin, greenLedPin);
  adjustLedValue(bluePotPin, blueLedPin);
}

/* 
  Reads the potentiometer value through the first given pin and, based on it, 
  adjusts the LED brightness through the second given pin. 
*/
void adjustLedValue(int potPin, int ledPin) {
  int potValue = analogRead(potPin);
  int newLedValue = map(potValue, minPotValue, maxPotValue, minLedValue, maxLedValue);
  analogWrite(ledPin, newLedValue);
}
