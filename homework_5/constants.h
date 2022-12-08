#pragma once

#include <Arduino.h>

namespace Constants {

  inline constexpr uint8_t buzzerPin = 9;
  inline constexpr uint8_t joyXPin = A0;
  inline constexpr uint8_t joyYPin = A1;
  inline constexpr uint8_t joySWPin = 2;
  inline constexpr uint8_t matrixDINPin = 13;
  inline constexpr uint8_t matrixClockPin = 12;
  inline constexpr uint8_t matrixLoadPin = 11;
  inline constexpr uint8_t lcdRSPin = 1;
  inline constexpr uint8_t lcdEnablePin = 8;
  inline constexpr uint8_t lcdD4Pin = 7;
  inline constexpr uint8_t lcdD5Pin = 6;
  inline constexpr uint8_t lcdD6Pin = 5;
  inline constexpr uint8_t lcdD7Pin = 4;
  inline constexpr uint8_t lcdContrastPin = 3;
  inline constexpr uint8_t lcdBrightnessPin = 10;

  inline constexpr uint8_t usernameAddr = 0;
  inline constexpr uint8_t usernameSize = 15;
  inline constexpr uint8_t contrastAddr = usernameSize;
  inline constexpr uint8_t lcdBrightAddr = contrastAddr + 1;
  inline constexpr uint8_t matrixBrightAddr = lcdBrightAddr + 1;
  inline constexpr uint8_t soundsOnAddr = matrixBrightAddr + 1;
  inline constexpr uint8_t highscoresAddr = soundsOnAddr + 1;
  inline constexpr uint8_t noHighscores = 5;
}
