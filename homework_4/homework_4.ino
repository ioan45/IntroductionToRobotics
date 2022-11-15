enum Display : uint8_t {D1, D2, D3, D4};
enum JoyDirection : uint8_t {LEFT, RIGHT, UP, DOWN, NEUTRAL};
enum ResetPhase : uint8_t {CAN_RESET, RESET_PERFORMED, RESET_REQUESTED};

const int joyPinX = A0;
const int joyPinY = A1;
const int joyPinSW = 2;
const int shiftClockPin = 10;
const int storageClockPin = 11;
const int dataPin = 12;
const int displayPins[] = {
  [Display::D1] = 7,
  [Display::D2] = 6,
  [Display::D3] = 5,
  [Display::D4] = 4
};
byte digitsEncodings[] = {   
  // Each value represents the state of each segment of a display digit so that a hex digit is shown. 
  // The segments in a value are ordered from A to G with DP at the end.
  // The hex digits in the array are ordered from 0 to F.
  B11111100,
  B01100000,
  B11011010,
  B11110010,
  B01100110,
  B10110110,
  B10111110,
  B11100000,
  B11111110,
  B11110110,
  B11101110,
  B00111110,
  B10011100,
  B01111010,
  B10011110,
  B10001110
};
const int nrDisplays = 4;
const int nrDigits = 16;
const int joyLowerThreshold = 400;
const int joyHigherThreshold = 600;
const int debounceDelay = 50;
const int resetPressDur = 3000;
const int blinkingSpeed = 300;

byte stateOfDisplays[nrDisplays];    // Contains the current encoding for each display. The displays are ordered from D1 to D4.
int encodingsIndices[nrDisplays];    // Contains indices of the digitsEncodings array referring to the digits encoded in the stateOfDisplays array.
byte currentJoySWState = HIGH;
byte blinkingState = LOW;
bool newJoyDirection = false;
bool switchProcessState = false;
int currentProcessState = 1;
int selectedDisplay = Display::D1;
JoyDirection currentJoyDir = NEUTRAL;
ResetPhase resetFlag = CAN_RESET;
long lastBlinkingTime = 0;
volatile long lastSWChangeTime = 0;


void setup() {
  pinMode(joyPinX, INPUT);
  pinMode(joyPinY, INPUT);
  pinMode(joyPinSW, INPUT_PULLUP);
  pinMode(shiftClockPin, OUTPUT);
  pinMode(storageClockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  for (int i = 0; i < nrDisplays; ++i) {
    pinMode(displayPins[i], OUTPUT);
    digitalWrite(displayPins[i], HIGH);
  }

  for (int i = 0; i < nrDisplays; ++i) {
    stateOfDisplays[i] = digitsEncodings[0];
    encodingsIndices[i] = 0;
  }

  attachInterrupt(digitalPinToInterrupt(joyPinSW), joyButtonISR, CHANGE);
}

void loop() {
  processInput();
  if (currentProcessState == 1)
    firstStateLogic();
  else
    secondStateLogic();
  renderOnDisplay();
}

void joyButtonISR() {
  lastSWChangeTime = millis();
}

void processInput() {
  // Joystick button (with debouncing) 
  byte currentSWReading = digitalRead(joyPinSW);
  if (millis() - lastSWChangeTime >= debounceDelay) {
    // The current signal (LOW or HIGH) is persistent enough to take it as actual state of the button.
    if (currentSWReading != currentJoySWState) {
      // Button state has changed. Update with the new state.
      currentJoySWState = currentSWReading;
      if (currentJoySWState == HIGH) {
        if (resetFlag == ResetPhase::RESET_PERFORMED)
          // During this finished press, a reset was performed. Set the flag for next press.
          resetFlag = ResetPhase::CAN_RESET;
        else
          // During this finished press, no reset was performed, so the state can be changed.
          switchProcessState = true;
      }
    }
    else if (currentJoySWState == LOW && currentProcessState == 1 && resetFlag == CAN_RESET && millis() - lastSWChangeTime >= resetPressDur)
      // No reset was performed during this press and the button was pressed for enough time to signal reset.
      resetFlag = ResetPhase::RESET_REQUESTED;
  }

  // Joystick direction (taking the first read direction)
  JoyDirection prevDir = currentJoyDir;
  int xReading = analogRead(joyPinX);
  int yReading = analogRead(joyPinY);
  if (xReading < joyLowerThreshold)
    currentJoyDir = JoyDirection::LEFT;
  else if (xReading > joyHigherThreshold)
    currentJoyDir = JoyDirection::RIGHT;
  else if (yReading < joyLowerThreshold)
    currentJoyDir = JoyDirection::UP;
  else if (yReading > joyHigherThreshold)
    currentJoyDir = JoyDirection::DOWN;
  else
    currentJoyDir = JoyDirection::NEUTRAL;
  
  // If this becomes true, it will stay like that until the next loop iteration. 
  // So, because the movement code is executed at most once per iteration, this can 
  // cause just one step displacement.
  newJoyDirection = (prevDir == JoyDirection::NEUTRAL && currentJoyDir != JoyDirection::NEUTRAL);
}

void firstStateLogic() {
  if (resetFlag == ResetPhase::RESET_REQUESTED) {
    for (int i = 0; i < nrDisplays; ++i) {
      stateOfDisplays[i] = digitsEncodings[0];
      encodingsIndices[i] = 0;
    }
    selectedDisplay = Display::D1;
    resetFlag = ResetPhase::RESET_PERFORMED;
    return;
  }
  
  if (switchProcessState) {
    stateOfDisplays[selectedDisplay] |= B00000001;  // it sets the last bit (the DP bit) to 1.
    currentProcessState = 2;
    switchProcessState = false;
    return;
  }

  if (millis() - lastBlinkingTime >= blinkingSpeed) {
    blinkingState = !blinkingState;
    // Last bit of the encoding (the DP bit) can be set to 1. So the display state is remade.
    stateOfDisplays[selectedDisplay] = digitsEncodings[encodingsIndices[selectedDisplay]] + blinkingState; 
    lastBlinkingTime = millis();
  }

  if (newJoyDirection && (currentJoyDir == JoyDirection::RIGHT || currentJoyDir == JoyDirection::LEFT)) {
    stateOfDisplays[selectedDisplay] &= B11111110;  // it sets the last bit (the DP bit) to 0.
    if (currentJoyDir == JoyDirection::RIGHT)
      selectedDisplay = (selectedDisplay - 1 + nrDisplays) % nrDisplays;
    else
      selectedDisplay = (selectedDisplay + 1) % nrDisplays;
    stateOfDisplays[selectedDisplay] |= B00000001;  // it sets the last bit (the DP bit) to 1.
    blinkingState = HIGH;
  }
}

void secondStateLogic() {
  if (switchProcessState) {
    currentProcessState = 1;
    switchProcessState = false;
    return;
  }

  if (newJoyDirection && (currentJoyDir == JoyDirection::UP || currentJoyDir == JoyDirection::DOWN)) {
    if (currentJoyDir == JoyDirection::UP)
      encodingsIndices[selectedDisplay] = (encodingsIndices[selectedDisplay] + 1) % nrDigits;
    else
      encodingsIndices[selectedDisplay] = (encodingsIndices[selectedDisplay] - 1 + nrDigits) % nrDigits;
    stateOfDisplays[selectedDisplay] = digitsEncodings[encodingsIndices[selectedDisplay]] + 1;  // "+1" to set the DP bit to 1.
  }
}

void renderOnDisplay() {
  for (int i = 0; i < nrDisplays; ++i) {   
    digitalWrite(displayPins[(i - 1 + nrDisplays) % nrDisplays], HIGH);
    writeReg(stateOfDisplays[i]);
    digitalWrite(displayPins[i], LOW);
    delay(1);  // To keep the current display digit ON for a bit more time. Otherwise, the LEDs will appear dimmer.
  }
}

void writeReg(byte displayEncoding) {
  digitalWrite(storageClockPin, LOW);
  shiftOut(dataPin, shiftClockPin, MSBFIRST, displayEncoding);
  digitalWrite(storageClockPin, HIGH);
}
