#pragma once

#include "LiquidCrystal.h"
#include "LedControl.h"

namespace Renderer {

  extern const uint8_t lcdWidth;
  extern const uint8_t matrixSize;
  extern LiquidCrystal lcdController;
  extern LedControl ledControl;
  extern bool soundsOn;

  void init();
  void addMatrixChange(uint8_t, uint8_t, uint8_t);
  void updateMatrix(); 
}