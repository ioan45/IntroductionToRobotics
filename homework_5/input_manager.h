#pragma once

#include <stdint.h>

namespace InputManager {
  
  enum class JoyDirection : uint8_t {NEUTRAL, LEFT, RIGHT, UP, DOWN};

  extern JoyDirection currentJoyDir;
  extern bool newJoyDir;
  extern bool buttonWasClicked;
  // extern bool buttonWasReleased;
  // extern bool buttonIsPressed;
  // extern uint32_t buttonPressDuration;

  void init();
  void getUserInput();
  void joyButtonISR();
}
