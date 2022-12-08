#include <Arduino.h>
#include "input_manager.h"
#include "renderer.h"
#include "menu.h"
#include "greetings.h"

void Greetings::start() {

  Renderer::lcdController.clear();
  Renderer::lcdController.setCursor(3, 0);
  Renderer::lcdController.print("Welcome to");
  Renderer::lcdController.setCursor(2, 1);
  Renderer::lcdController.print("Hungry Pixel");
  greetingStartTime = millis();
}

void Greetings::update() {
  constexpr uint8_t greetingDuration = 3;
  if (millis() - greetingStartTime >= greetingDuration * 1000)
    Scene::playNewScene<Menu>();
}