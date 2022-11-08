enum Segment: uint8_t {A, B, C, D, E, F, G, DP};
enum JoyDirection : uint8_t {LEFT, RIGHT, UP, DOWN, NEUTRAL};
enum ResetPhase: uint8_t {CAN_RESET, RESET_PERFORMED, RESET_REQUESTED};

const int joyLowerThreshold = 400;
const int joyHigherThreshold = 600;
const int debounceDelay = 50;
const int nrSegments = 8;
const int nrDirections = 4;
const int blinkingSpeed = 350;
const int resetPressDur = 3000;

const int joyPinX = A0;
const int joyPinY = A1;
const int joyPinSW = 2; 
const int segmentPins[nrSegments] = {
  [Segment::A] = 4,
  [Segment::B] = 5,
  [Segment::C] = 6,
  [Segment::D] = 7,
  [Segment::E] = 8,
  [Segment::F] = 9,
  [Segment::G] = 10,
  [Segment::DP] = 11
};

// Each line contains the neighbours (as indices of the segments pins array) of a segment. The segments are ordered from A to G with DP as the last line. 
// Each column represents a direction (LEFT, RIGHT, UP and DOWN, in this order).
const int8_t neighbours[nrSegments][nrDirections] = {
  {Segment::F, Segment::B,          -1, Segment::G},
  {Segment::F,         -1,  Segment::A, Segment::G},
  {Segment::E, Segment::DP, Segment::G, Segment::D},
  {Segment::E, Segment::C,  Segment::G,         -1},
  {        -1, Segment::C,  Segment::G, Segment::D},
  {        -1, Segment::B,  Segment::A, Segment::G},
  {        -1,         -1,  Segment::A, Segment::D},
  {Segment::C,         -1,          -1,         -1}
};
 
byte displayState[nrSegments];    // The state of each segment. The segments are ordered from A to G with DP as the last index. 
byte currentJoySWState = HIGH;
byte blinkingState = LOW;
bool newJoyDirection = false;
bool switchState = false;
int currentProcessState = 1;
long lastBlinkingTime = 0;
Segment blinkingSegment = Segment::DP;
JoyDirection currentJoyDir = NEUTRAL;
ResetPhase resetFlag = CAN_RESET;
volatile long lastSWChangeTime = 0;


void setup() {
  pinMode(joyPinX, INPUT);
  pinMode(joyPinY, INPUT);
  pinMode(joyPinSW, INPUT_PULLUP);
  for (int i = 0; i < nrSegments; ++i)
    pinMode(segmentPins[i], OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(joyPinSW), joyButtonISR, CHANGE);
}

void loop() {
  processInput();
  if (currentProcessState == 1)
    firstStateLogic();
  else
    secondStateLogic();
}

void joyButtonISR() {
  lastSWChangeTime = millis();
}

void processInput() {
  // Joystick button (with debouncing) 
  byte currentSWReading = digitalRead(joyPinSW);
  if (millis() - lastSWChangeTime >= debounceDelay) {
    if (currentSWReading != currentJoySWState) {
      // Button state has changed. Update with the new state.
      currentJoySWState = currentSWReading;
      if (currentJoySWState == HIGH) {
        if (resetFlag == RESET_PERFORMED)
          // During this finished press, a reset was performed. Set the flag for next press.
          resetFlag = CAN_RESET;
        else
          // During this finished press, no reset was performed, so the state can be changed.
          switchState = true;
      }
    }
    else if (currentProcessState == 1 && resetFlag == CAN_RESET && currentJoySWState == LOW && millis() - lastSWChangeTime >= resetPressDur)
      // No reset was performed during this press and the button was pressed for enough time to signal reset.
      resetFlag = RESET_REQUESTED;
  }

  // Joystick direction (taking the first read direction)
  JoyDirection prevDir = currentJoyDir;
  int xReading = analogRead(joyPinX);
  int yReading = analogRead(joyPinY);
  if (xReading < joyLowerThreshold)
    currentJoyDir = LEFT;
  else if (xReading > joyHigherThreshold)
    currentJoyDir = RIGHT;
  else if (yReading < joyLowerThreshold)
    currentJoyDir = UP;
  else if (yReading > joyHigherThreshold)
    currentJoyDir = DOWN;
  else
    currentJoyDir = NEUTRAL;
  
  // If this becomes true, it will stay like that until the next loop iteration. 
  // So, because the movement code is executed at most once per iteration, this can 
  // cause just one step displacement.
  newJoyDirection = (prevDir == NEUTRAL && currentJoyDir != NEUTRAL);
}

void firstStateLogic() {
  if (resetFlag == RESET_REQUESTED) {
    for (int i = 0; i < nrSegments; ++i) {
      displayState[i] = LOW;
      digitalWrite(segmentPins[i], LOW);
    }
    blinkingSegment = Segment::DP; 
    resetFlag = RESET_PERFORMED;
    return;
  }
  
  if (switchState) {
    displayState[blinkingSegment] = blinkingState;
    currentProcessState = 2;
    switchState = false;
    return;
  }

  if (millis() - lastBlinkingTime >= blinkingSpeed) {
    blinkingState = !blinkingState;
    digitalWrite(segmentPins[blinkingSegment], blinkingState);
    lastBlinkingTime = millis();
  }
  
  if (newJoyDirection) {
    int8_t neighbour = neighbours[blinkingSegment][currentJoyDir];
    if (neighbour != -1) {
      digitalWrite(segmentPins[blinkingSegment], displayState[blinkingSegment]);
      blinkingSegment = (Segment)neighbour;
    }
  }
}

void secondStateLogic() {
  if (switchState) {
    currentProcessState = 1;
    switchState = false;
    return;
  }

  if (newJoyDirection && (currentJoyDir == UP || currentJoyDir == DOWN)) {
    displayState[blinkingSegment] = !displayState[blinkingSegment];
    digitalWrite(segmentPins[blinkingSegment], displayState[blinkingSegment]);
  }
}
