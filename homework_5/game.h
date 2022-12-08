#pragma once

#include "scene.h"

class Game : Scene {
  public:
    void start();
    void update();
    void checkMovement();
    void checkTarget();
    void generateTargetPos();
    void checkGameOver();
    Game();
  
  private:
    uint8_t pointLin, pointCol, targetLin, targetCol;
    uint32_t score;
    uint32_t lastMoveTime;
    const uint16_t maxStandStillDur;
};