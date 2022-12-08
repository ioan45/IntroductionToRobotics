#include <EEPROM.h>
#include "ArduinoQueue.h"
#include "constants.h"
#include "renderer.h"

namespace Renderer {

  struct LedChange {
    uint8_t line;
    uint8_t column;
    uint8_t newValue;
  };

  const uint8_t matrixSize = 8;
  ArduinoQueue<LedChange> matrixChanges(matrixSize * matrixSize);
  LedControl ledControl (
    Constants::matrixDINPin, 
    Constants::matrixClockPin, 
    Constants::matrixLoadPin, 
    1
  );
  
  const uint8_t lcdWidth = 16;
  LiquidCrystal lcdController (
    Constants::lcdRSPin,
    Constants::lcdEnablePin,
    Constants::lcdD4Pin,
    Constants::lcdD5Pin,
    Constants::lcdD6Pin,
    Constants::lcdD7Pin
  );

  bool soundsOn;

  void init() {
    pinMode(Constants::matrixDINPin, OUTPUT);
    pinMode(Constants::matrixClockPin, OUTPUT);
    pinMode(Constants::matrixLoadPin, OUTPUT);
    pinMode(Constants::lcdRSPin, OUTPUT);
    pinMode(Constants::lcdEnablePin, OUTPUT);
    pinMode(Constants::lcdD4Pin, OUTPUT);
    pinMode(Constants::lcdD5Pin, OUTPUT);
    pinMode(Constants::lcdD6Pin, OUTPUT);
    pinMode(Constants::lcdD7Pin, OUTPUT);
    pinMode(Constants::lcdContrastPin, OUTPUT);
    pinMode(Constants::lcdBrightnessPin, OUTPUT);

    //pinMode(Constants::buzzerPin, OUTPUT);

    lcdController.begin(16, 2);
    ledControl.shutdown(0, false);
    ledControl.clearDisplay(0);

    uint8_t eepromValue;
    EEPROM.get(Constants::contrastAddr, eepromValue);
    analogWrite(Constants::lcdContrastPin, eepromValue);
    EEPROM.get(Constants::lcdBrightAddr, eepromValue);
    analogWrite(Constants::lcdBrightnessPin, eepromValue);
    EEPROM.get(Constants::matrixBrightAddr, eepromValue);
    ledControl.setIntensity(0, eepromValue);
    EEPROM.get(Constants::soundsOnAddr, soundsOn);
  }

  void addMatrixChange(uint8_t line, uint8_t column, uint8_t newValue) {
    matrixChanges.enqueue({line, column, newValue});
  }

  void updateMatrix() {
    LedChange tmp;
    while (!matrixChanges.isEmpty()) {
      tmp = matrixChanges.dequeue();
      ledControl.setLed(0, tmp.line, tmp.column, tmp.newValue);
    }
  }
}
