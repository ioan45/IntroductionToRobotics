const int semaphoreButtonPin = 2;
const int buzzerPin = 5;
const int peopleGreenPin = 6;
const int peopleRedPin = 7;
const int carGreenPin = 8;
const int carYellowPin = 9;
const int carRedPin = 10;

const int carGreenDur = 8000;
const int carYellowDur = 3000;
const int peopleGreenDur = 8000;
const int greenBlinkingDur = 4000;
const int greenBlinkingStateDelay = 300;
const int buzzerNormalToneDur = 800;
const int buzzerNormalPauseDur = 800;
const int buzzerNormalFrequency = 800;
const int buzzerFastToneDur = 300;
const int buzzerFastPauseDur = 300;
const int buzzerFastFrequency = 1200;
const int debounceDelay = 50;

long lastTransitionTime;
long lastToneTime;
long lastBlinkingChangeTime;
byte ledBlinkingState;
long buttonPressTime;
byte lastButtonState = HIGH;
void (*currentStateLogic)() = NULL;

volatile long lastSignalChangeTime = 0;

void setup() {
  pinMode(semaphoreButtonPin, INPUT_PULLUP);
  pinMode(peopleRedPin, OUTPUT);
  pinMode(peopleGreenPin, OUTPUT);
  pinMode(carRedPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carGreenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(semaphoreButtonPin), buttonISR, CHANGE);

  // Set up the default state
  digitalWrite(peopleRedPin, HIGH);
  digitalWrite(carGreenPin, HIGH);
}

void loop() {
  // -- Button debouncing --
  byte currentButtonState = digitalRead(semaphoreButtonPin);
  if (millis() - lastSignalChangeTime >= debounceDelay && currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    // Button state has changed. Start the process if it's not already running and the button it's pressed. 
    if (!currentStateLogic && currentButtonState == LOW) {
      buttonPressTime = lastSignalChangeTime;
      currentStateLogic = firstStateLogic;
    }
  }

  if (currentStateLogic)
    currentStateLogic();
}

void buttonISR() {
  lastSignalChangeTime = millis();
}

void firstStateLogic() {
  // Transition to the next state
  if (millis() - buttonPressTime >= carGreenDur) {
    digitalWrite(carGreenPin, LOW);
    digitalWrite(carYellowPin, HIGH);
    currentStateLogic = secondStateLogic;
    lastTransitionTime = millis();
  }
}

void secondStateLogic() {
  // Transition to the next state
  if (millis() - lastTransitionTime >= carYellowDur) {
    digitalWrite(carYellowPin, LOW);
    digitalWrite(carRedPin, HIGH);
    digitalWrite(peopleRedPin, LOW);
    digitalWrite(peopleGreenPin, HIGH);
    lastToneTime = 0;
    currentStateLogic = thirdStateLogic;
    lastTransitionTime = millis();
  }
}

void thirdStateLogic() {
  // Buzzer normal beeping sound
  if (millis() - lastToneTime >= buzzerNormalToneDur + buzzerNormalPauseDur) {
    tone(buzzerPin, buzzerNormalFrequency, buzzerNormalToneDur);
    lastToneTime = millis();
  }

  // Transition to the next state
  if (millis() - lastTransitionTime >= peopleGreenDur) {
    lastToneTime = 0;
    noTone(buzzerPin);
    digitalWrite(peopleGreenPin, LOW);
    ledBlinkingState = LOW;
    lastBlinkingChangeTime = 0;
    currentStateLogic = fourthStateLogic;
    lastTransitionTime = millis();
  }
}

void fourthStateLogic() {
  // Blinking green light
  if (millis() - lastBlinkingChangeTime >= greenBlinkingStateDelay) {
    ledBlinkingState = !ledBlinkingState;
    digitalWrite(peopleGreenPin, ledBlinkingState);
    lastBlinkingChangeTime = millis();
  }
  
  // Buzzer fast beeping sound
  if (millis() - lastToneTime >= buzzerFastToneDur + buzzerFastPauseDur) {
    tone(buzzerPin, buzzerFastFrequency, buzzerFastToneDur);
    lastToneTime = millis();
  }

  // Transition to the default state
  if (millis() - lastTransitionTime >= greenBlinkingDur) {
    digitalWrite(peopleGreenPin, LOW);
    digitalWrite(peopleRedPin, HIGH);
    digitalWrite(carRedPin, LOW);
    digitalWrite(carGreenPin, HIGH);
    noTone(buzzerPin);
    currentStateLogic = NULL;
  }
}
