#pragma once

#include "scene.h"

class Greetings : Scene {
  public:
    void start();
    void update();

  private:
    uint32_t greetingStartTime;
};