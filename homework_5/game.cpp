#include "input_manager.h"
#include "renderer.h"
#include "game.h"
#include "menu.h"

Game::Game()
  : maxStandStillDur(7000)
{
}

void Game::start() {
  score = 0;
  lastMoveTime = millis();
  Renderer::lcdController.print("Score: 0");

  // initialize point
  pointLin = pointCol = Renderer::matrixSize / 2;
  Renderer::addMatrixChange(pointLin, pointCol, HIGH);

  // initialize target
  uint8_t freeAnalogPin = A4;
  randomSeed(analogRead(freeAnalogPin));
  generateTargetPos();
  Renderer::addMatrixChange(targetLin, targetCol, HIGH);
}

void Game::update() {
  checkMovement();
  checkTarget();
  checkGameOver();
}

void Game::checkMovement() {
  if (InputManager::newJoyDir) {
    Renderer::addMatrixChange(pointLin, pointCol, LOW);
    if (InputManager::currentJoyDir == InputManager::JoyDirection::LEFT)
      pointCol = (pointCol - 1 + Renderer::matrixSize) % Renderer::matrixSize;
    else if (InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT)
      pointCol = (pointCol + 1) % Renderer::matrixSize;
    else if (InputManager::currentJoyDir == InputManager::JoyDirection::UP)
      pointLin = (pointLin - 1 + Renderer::matrixSize) % Renderer::matrixSize;
    else
      pointLin = (pointLin + 1) % Renderer::matrixSize;
    Renderer::addMatrixChange(pointLin, pointCol, HIGH);
    lastMoveTime = millis();
  }
}

void Game::checkTarget() {
  if (pointLin == targetLin && pointCol == targetCol) {
    ++score;
    Renderer::lcdController.clear();
    Renderer::lcdController.print("Score: ");
    Renderer::lcdController.print(score);
    generateTargetPos();
    Renderer::addMatrixChange(targetLin, targetCol, HIGH);
  }
}

void Game::generateTargetPos() {
  targetCol = random(Renderer::matrixSize);
  do {
    targetLin = targetCol;
    targetCol = random(Renderer::matrixSize);
  } while (targetLin == pointLin && targetCol == pointCol);
}

void Game::checkGameOver() {
  if (millis() - lastMoveTime >= maxStandStillDur) {
    Renderer::ledControl.clearDisplay(0);
    Scene::playNewScene<Menu>();
  }
}
