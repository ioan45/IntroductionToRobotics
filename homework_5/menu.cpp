#include <Arduino.h>
#include <EEPROM.h>
#include <stdlib.h>
#include "constants.h"
#include "input_manager.h"
#include "renderer.h"
#include "game.h"
#include "menu.h"

Menu::Menu() 
  : unitsPerBlock(16), scrollingTextSpeed(600), noMainMenuOptions(5), noAboutLines(3), 
  noHowToTextLines(4), noSettings(7)
{
}

Menu::~Menu() {
  delete[] mainMenuOptions;
  delete[] aboutLines;
  delete[] howToTextLines;
  delete[] settings;
}

void Menu::start() {
  mainMenuOptions = new const char*[noMainMenuOptions] {
    "Play", 
    "How to play", 
    "Highscores", 
    "Settings", 
    "About"
  };
  aboutLines = new const char*[noAboutLines] {
    "Game title: Hungry Pixel",
    "GitHub: https://github.com/ioan45",
    "Go back to menu"
  };
  howToTextLines = new const char*[noHowToTextLines] {
    "Earn points",
    "by \'eating the",
    "food\'.",
    "Go back to menu"
  };
  settings = new const char*[noSettings] {
    "Enter name",
    "Difficulty",
    "LCD Contrast",
    "LCD brightness",
    "Matrix brightness",
    "Sounds ON/OFF",
    "Go back to menu"
  };

  currentUpdateAction = &Menu::onMainMenu;
  mainMenuPointer.pointedOptionIndex = 0;
  mainMenuPointer.displayLine = 0;
  scrollingTextIndex = 0;
  currentOptionLength = strlen(mainMenuOptions[0]);
  displayVisibleOptions(mainMenuOptions, mainMenuPointer);
}

void Menu::update() {
  if (currentUpdateAction)
    (this->*currentUpdateAction)();
}

void Menu::onMainMenu() {
  bool scrolled = checkOptionsScrolling(noMainMenuOptions, mainMenuOptions, mainMenuPointer);
  checkTextScrolling(scrolled, mainMenuOptions[mainMenuPointer.pointedOptionIndex], mainMenuPointer.displayLine);

  if (InputManager::buttonWasClicked) {
    switch (mainMenuPointer.pointedOptionIndex) {
      case 0:
        Renderer::lcdController.clear();
        Scene::playNewScene<Game>();
        break;
      case 1:
        currentUpdateAction = &Menu::onHowToPlayOption;
        selectedOptPointer.displayLine = 0;
        selectedOptPointer.pointedOptionIndex = 0;
        currentOptionLength = strlen(howToTextLines[0]);
        lastTextScrollTime = millis();
        displayVisibleOptions(howToTextLines, selectedOptPointer);
        break;
      case 2:
        highscoresLines = new char*[Constants::noHighscores + 1];  // + 1 for "Go back to menu"
        // char highscoreNames[Constants::noHighscores][Constants::usernameSize];
        // uint32_t* highscoreValues[Constants::noHighscores];
        for (uint8_t i = 0; i < Constants::noHighscores; ++i) {
          constexpr uint8_t maxScoreDigits = 10;
          char scoreName[Constants::usernameSize];
          uint32_t scoreValue;
          highscoresLines[i] = new char[Constants::usernameSize + maxScoreDigits + 2];  // + 2 for space character and null terminator in case of max size text
          const uint8_t currentScoreNameAddr = Constants::highscoresAddr + i * Constants::usernameSize + i * sizeof(scoreValue);
          EEPROM.get(currentScoreNameAddr, scoreName);
          EEPROM.get(currentScoreNameAddr + Constants::usernameSize, scoreValue);
          strcpy(highscoresLines[i], scoreName);
          highscoresLines[i][Constants::usernameSize] = ' ';
          ultoa(scoreValue, highscoresLines[i] + Constants::usernameSize + 1, 10);  // converts number in base 10
        }
        highscoresLines[Constants::noHighscores] = new char[15];  // for "Go back to menu"
        strcpy(highscoresLines[Constants::noHighscores], "Go back to menu");
        
        currentUpdateAction = &Menu::onHighscoresOption;
        selectedOptPointer.pointedOptionIndex = 0;
        selectedOptPointer.displayLine = 0;
        currentOptionLength = strlen(highscoresLines[0]);
        lastTextScrollTime = millis();
        displayVisibleOptions((const char**)highscoresLines, selectedOptPointer);
        break;
      case 3:
        currentUpdateAction = &Menu::onSettingsOption;
        settingsPointer.pointedOptionIndex = 0;
        settingsPointer.displayLine = 0;
        currentOptionLength = strlen(settings[0]);
        lastTextScrollTime = millis();
        displayVisibleOptions(settings, settingsPointer);
        break;
      case 4:
        currentUpdateAction = &Menu::onAboutOption;
        selectedOptPointer.displayLine = 0;
        selectedOptPointer.pointedOptionIndex = 0;
        currentOptionLength = strlen(aboutLines[0]);
        lastTextScrollTime = millis();
        displayVisibleOptions(aboutLines, selectedOptPointer);
        break;
    }
  }
}

void Menu::onHowToPlayOption() {
  bool scrolled = checkOptionsScrolling(noHowToTextLines, howToTextLines, selectedOptPointer);
  checkTextScrolling(scrolled, howToTextLines[selectedOptPointer.pointedOptionIndex], selectedOptPointer.displayLine);
  if (InputManager::buttonWasClicked && selectedOptPointer.pointedOptionIndex == noHowToTextLines - 1) {
    currentUpdateAction = &Menu::onMainMenu;
    displayVisibleOptions(mainMenuOptions, mainMenuPointer);
  }
}

void Menu::onHighscoresOption() {
  bool scrolled = checkOptionsScrolling(Constants::noHighscores + 1, (const char**)highscoresLines, selectedOptPointer);
  checkTextScrolling(scrolled, (const char*)highscoresLines[selectedOptPointer.pointedOptionIndex], selectedOptPointer.displayLine);
  if (InputManager::buttonWasClicked && selectedOptPointer.pointedOptionIndex == Constants::noHighscores) {
    for (uint8_t i = 0; i <= Constants::noHighscores; ++i)
      delete[] highscoresLines[i];
    delete[] highscoresLines;
    currentUpdateAction = &Menu::onMainMenu;
    displayVisibleOptions(mainMenuOptions, mainMenuPointer);
  }
}

void Menu::onSettingsOption() {
  bool scrolled = checkOptionsScrolling(noSettings, settings, settingsPointer);
  checkTextScrolling(scrolled, settings[settingsPointer.pointedOptionIndex], settingsPointer.displayLine);
  if (InputManager::buttonWasClicked) {
    if (settingsPointer.pointedOptionIndex == 0) {
      currentUpdateAction = &Menu::onEnterNameSetting;
      EEPROM.get(Constants::usernameAddr, username);
      selectedOptPointer.displayLine = 0;
      selectedOptPointer.pointedOptionIndex = 0;
      cursorCurrentColumn = 0;
      Renderer::lcdController.clear();
      Renderer::lcdController.print('>');
      Renderer::lcdController.print(username);
      Renderer::lcdController.setCursor(1, 1);
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);
    }
    else if (settingsPointer.pointedOptionIndex == 2) {
      currentUpdateAction = &Menu::onContrastSetting;
      EEPROM.get(Constants::contrastAddr, contrastValue);
      selectedOptPointer.displayLine = 0;
      selectedOptPointer.pointedOptionIndex = 0;
      locked = false;
      Renderer::lcdController.clear();
      Renderer::lcdController.print('>');
      for (uint8_t i = 0; i < contrastValue / unitsPerBlock; ++i)
        Renderer::lcdController.write(B11111111);  // Fill block
      Renderer::lcdController.setCursor(1, 1);
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);      
    }
    else if (settingsPointer.pointedOptionIndex == 3) {
      currentUpdateAction = &Menu::onLcdBrightSetting;
      EEPROM.get(Constants::lcdBrightAddr, lcdBrightValue);
      selectedOptPointer.displayLine = 0;
      selectedOptPointer.pointedOptionIndex = 0;
      locked = false;
      Renderer::lcdController.clear();
      Renderer::lcdController.print('>');
      for (uint8_t i = 0; i < lcdBrightValue / unitsPerBlock; ++i)
        Renderer::lcdController.write(B11111111);  // Fill block
      Renderer::lcdController.setCursor(1, 1);
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);      
    }
    else if (settingsPointer.pointedOptionIndex == 4) {
      currentUpdateAction = &Menu::onMatrixBrightSetting;
      EEPROM.get(Constants::matrixBrightAddr, matrixBrightValue);
      selectedOptPointer.displayLine = 0;
      selectedOptPointer.pointedOptionIndex = 0;
      lastTextScrollTime = millis();
      currentOptionLength = Renderer::lcdWidth - 1;
      locked = false;
      for (uint8_t i = 0; i < Renderer::matrixSize; ++i)
        for (uint8_t j = 0; j < Renderer::matrixSize; ++j)
          Renderer::addMatrixChange(i, j, HIGH);
      Renderer::lcdController.clear();
      Renderer::lcdController.print('>');
      for (uint8_t i = 0; i < matrixBrightValue; ++i)
        Renderer::lcdController.write(B11111111);  // Fill block
      Renderer::lcdController.setCursor(1, 1);
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);      
    }
    else if (settingsPointer.pointedOptionIndex == 5) {
      currentUpdateAction = &Menu::onMuteSoundsSetting;
      selectedOptPointer.displayLine = 0;
      selectedOptPointer.pointedOptionIndex = 0;
      locked = false;
      Renderer::lcdController.clear();
      Renderer::lcdController.print('>');
      if (Renderer::soundsOn)
        Renderer::lcdController.print("ON");
      else
        Renderer::lcdController.print("OFF");
      Renderer::lcdController.setCursor(1, 1);
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);  
    }
    else if (settingsPointer.pointedOptionIndex == noSettings - 1) {
      currentUpdateAction = &Menu::onMainMenu;
      displayVisibleOptions(mainMenuOptions, mainMenuPointer);
    }
  }  
}

void Menu::onEnterNameSetting() {
  bool scrolled = false;

  if (InputManager::newJoyDir) {
    if (InputManager::currentJoyDir == InputManager::JoyDirection::UP) {
      if (selectedOptPointer.displayLine == 1) {
        selectedOptPointer.displayLine = 0;
        Renderer::lcdController.setCursor(0, 1);
        Renderer::lcdController.print(' ');
        Renderer::lcdController.print("Go back to settings");
        Renderer::lcdController.setCursor(0, 0);
        Renderer::lcdController.print('>');
        currentOptionLength = Constants::usernameSize;
      }
      else if (cursorCurrentColumn > 0) {
        if (username[cursorCurrentColumn - 1] == 'A')
          username[cursorCurrentColumn - 1] = ' ';
        else if (username[cursorCurrentColumn - 1] == ' ')
          username[cursorCurrentColumn - 1] = 'Z';
        else
          --username[cursorCurrentColumn - 1];
        Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
        Renderer::lcdController.print(username[cursorCurrentColumn - 1]);
        Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
      }
    }
    else if (InputManager::currentJoyDir == InputManager::JoyDirection::DOWN) {
      if (cursorCurrentColumn > 0) {
        if (username[cursorCurrentColumn - 1] == 'Z')
          username[cursorCurrentColumn - 1] = ' ';
        else if (username[cursorCurrentColumn - 1] == ' ')
          username[cursorCurrentColumn - 1] = 'A';
        else
          ++username[cursorCurrentColumn - 1];
        Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
        Renderer::lcdController.print(username[cursorCurrentColumn - 1]);
        Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
      }
      else if (selectedOptPointer.displayLine == 0) {
        selectedOptPointer.displayLine = 1;
        Renderer::lcdController.setCursor(0, 0);
        Renderer::lcdController.print(' ');
        Renderer::lcdController.setCursor(0, 1);
        Renderer::lcdController.print('>');
        scrolled = true;
        currentOptionLength = 19;  // Same as strlen("Go back to settings");
      }
    }    
    else if (InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT && 
              selectedOptPointer.displayLine == 0 && cursorCurrentColumn < Renderer::lcdWidth - 1) {
      if (cursorCurrentColumn == 0) {
        Renderer::lcdController.print(' ');
        Renderer::lcdController.cursor();
      }
      ++cursorCurrentColumn;
      Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
    }
    else if (InputManager::currentJoyDir == InputManager::JoyDirection::LEFT && 
             selectedOptPointer.displayLine == 0 && cursorCurrentColumn > 0) {
      --cursorCurrentColumn;
      Renderer::lcdController.setCursor(cursorCurrentColumn, selectedOptPointer.displayLine);
      if (cursorCurrentColumn == 0) {
        Renderer::lcdController.noCursor();
        Renderer::lcdController.print('>');
      }
    }
  }
  if (InputManager::buttonWasClicked && selectedOptPointer.displayLine == 1) {
    EEPROM.put(Constants::usernameAddr, username);
    currentUpdateAction = &Menu::onSettingsOption;
    lastTextScrollTime = millis();
    currentOptionLength = strlen(settings[settingsPointer.pointedOptionIndex]);
    displayVisibleOptions(settings, settingsPointer);
  }

  checkTextScrolling(scrolled, "Go back to settings", selectedOptPointer.displayLine);
}

void Menu::onContrastSetting() {
  bool scrolled = false;

  if (InputManager::buttonWasClicked && selectedOptPointer.displayLine == 0)
    locked = !locked;

  if (InputManager::newJoyDir) {
    if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::UP && selectedOptPointer.displayLine == 1) {
      selectedOptPointer.displayLine = 0;
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print('>');
      currentOptionLength = Renderer::lcdWidth - 1;
    }
    else if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::DOWN && selectedOptPointer.displayLine == 0) {
      selectedOptPointer.displayLine = 1;
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print('>');
      scrolled = true;
      currentOptionLength = 19;  // Same as strlen("Go back to settings");
    }    
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT && 
             selectedOptPointer.displayLine == 0 && contrastValue / unitsPerBlock < Renderer::lcdWidth - 1) {
      contrastValue += unitsPerBlock;
      Renderer::lcdController.setCursor(contrastValue / unitsPerBlock, selectedOptPointer.displayLine);
      Renderer::lcdController.write(B11111111);  // Fill block
      analogWrite(Constants::lcdContrastPin, contrastValue);
    }
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::LEFT && 
             selectedOptPointer.displayLine == 0 && contrastValue / unitsPerBlock > 0) {
      Renderer::lcdController.setCursor(contrastValue / unitsPerBlock, selectedOptPointer.displayLine);
      Renderer::lcdController.print(' ');
      contrastValue -= unitsPerBlock;
      analogWrite(Constants::lcdContrastPin, contrastValue);
    }
  }
  if (!locked && InputManager::buttonWasClicked && selectedOptPointer.displayLine == 1) {
    EEPROM.put(Constants::contrastAddr, contrastValue);
    currentUpdateAction = &Menu::onSettingsOption;
    lastTextScrollTime = millis();
    currentOptionLength = strlen(settings[settingsPointer.pointedOptionIndex]);
    displayVisibleOptions(settings, settingsPointer);
  }

  checkTextScrolling(scrolled, "Go back to settings", selectedOptPointer.displayLine);
}

void Menu::onLcdBrightSetting() {
  bool scrolled = false;

  if (InputManager::buttonWasClicked && selectedOptPointer.displayLine == 0)
    locked = !locked;

  if (InputManager::newJoyDir) {
    if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::UP && selectedOptPointer.displayLine == 1) {
      selectedOptPointer.displayLine = 0;
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print('>');
      currentOptionLength = Renderer::lcdWidth - 1;
    }
    else if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::DOWN && selectedOptPointer.displayLine == 0) {
      selectedOptPointer.displayLine = 1;
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print('>');
      scrolled = true;
      currentOptionLength = 19;  // Same as strlen("Go back to settings");
    }    
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT && 
             selectedOptPointer.displayLine == 0 && lcdBrightValue / unitsPerBlock < Renderer::lcdWidth - 1) {
      lcdBrightValue += unitsPerBlock;
      Renderer::lcdController.setCursor(lcdBrightValue / unitsPerBlock, selectedOptPointer.displayLine);
      Renderer::lcdController.write(B11111111);  // Fill block
      analogWrite(Constants::lcdBrightnessPin, lcdBrightValue);
    }
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::LEFT && 
             selectedOptPointer.displayLine == 0 && lcdBrightValue / unitsPerBlock > 0) {
      Renderer::lcdController.setCursor(lcdBrightValue / unitsPerBlock, selectedOptPointer.displayLine);
      Renderer::lcdController.print(' ');
      lcdBrightValue -= unitsPerBlock;
      analogWrite(Constants::lcdBrightnessPin, lcdBrightValue);
    }
  }
  if (!locked && InputManager::buttonWasClicked && selectedOptPointer.displayLine == 1) {
    EEPROM.put(Constants::lcdBrightAddr, lcdBrightValue);
    currentUpdateAction = &Menu::onSettingsOption;
    lastTextScrollTime = millis();
    currentOptionLength = strlen(settings[settingsPointer.pointedOptionIndex]);
    displayVisibleOptions(settings, settingsPointer);
  }

  checkTextScrolling(scrolled, "Go back to settings", selectedOptPointer.displayLine);
}

void Menu::onMatrixBrightSetting() {
  bool scrolled = false;

  if (InputManager::buttonWasClicked && selectedOptPointer.displayLine == 0)
    locked = !locked;

  if (InputManager::newJoyDir) {
    if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::UP && selectedOptPointer.displayLine == 1) {
      selectedOptPointer.displayLine = 0;
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print('>');
      currentOptionLength = Renderer::lcdWidth - 1;
    }
    else if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::DOWN && selectedOptPointer.displayLine == 0) {
      selectedOptPointer.displayLine = 1;
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print('>');
      scrolled = true;
      currentOptionLength = 19;  // Same as strlen("Go back to settings");
    }    
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT && 
             selectedOptPointer.displayLine == 0 && matrixBrightValue < Renderer::lcdWidth - 1) {
      ++matrixBrightValue;
      Renderer::lcdController.setCursor(matrixBrightValue, selectedOptPointer.displayLine);
      Renderer::lcdController.write(B11111111);  // Fill block
      Renderer::ledControl.setIntensity(0, matrixBrightValue);
    }
    else if (locked && InputManager::currentJoyDir == InputManager::JoyDirection::LEFT && 
             selectedOptPointer.displayLine == 0 && matrixBrightValue > 0) {
      Renderer::lcdController.setCursor(matrixBrightValue, selectedOptPointer.displayLine);
      Renderer::lcdController.print(' ');
      --matrixBrightValue;
      Renderer::ledControl.setIntensity(0, matrixBrightValue);
    }
  }
  if (!locked && InputManager::buttonWasClicked && selectedOptPointer.displayLine == 1) {
    EEPROM.put(Constants::matrixBrightAddr, matrixBrightValue);
    currentUpdateAction = &Menu::onSettingsOption;
    lastTextScrollTime = millis();
    currentOptionLength = strlen(settings[settingsPointer.pointedOptionIndex]);
    for (uint8_t i = 0; i < Renderer::matrixSize; ++i)
        for (uint8_t j = 0; j < Renderer::matrixSize; ++j)
          Renderer::addMatrixChange(i, j, LOW);
    displayVisibleOptions(settings, settingsPointer);
  }

  checkTextScrolling(scrolled, "Go back to settings", selectedOptPointer.displayLine);
}

void Menu::onMuteSoundsSetting() {
  bool scrolled = false;

  if (InputManager::buttonWasClicked && selectedOptPointer.displayLine == 0)
    locked = !locked;

  if (InputManager::newJoyDir) {
    if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::UP && selectedOptPointer.displayLine == 1) {
      selectedOptPointer.displayLine = 0;
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.print("Go back to settings");
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print('>');
      currentOptionLength = Renderer::lcdWidth - 1;
    }
    else if (!locked && InputManager::currentJoyDir == InputManager::JoyDirection::DOWN && selectedOptPointer.displayLine == 0) {
      selectedOptPointer.displayLine = 1;
      Renderer::lcdController.setCursor(0, 0);
      Renderer::lcdController.print(' ');
      Renderer::lcdController.setCursor(0, 1);
      Renderer::lcdController.print('>');
      scrolled = true;
      currentOptionLength = 19;  // Same as strlen("Go back to settings");
    }    
    else if (locked && (InputManager::currentJoyDir == InputManager::JoyDirection::RIGHT || 
             InputManager::currentJoyDir == InputManager::JoyDirection::LEFT) && selectedOptPointer.displayLine == 0) {
      Renderer::soundsOn = !Renderer::soundsOn;
      Renderer::lcdController.setCursor(1, selectedOptPointer.displayLine);
      if (Renderer::soundsOn)
        Renderer::lcdController.print("ON ");
      else
        Renderer::lcdController.print("OFF");
    }
  }
  if (!locked && InputManager::buttonWasClicked && selectedOptPointer.displayLine == 1) {
    EEPROM.put(Constants::soundsOnAddr, Renderer::soundsOn);
    currentUpdateAction = &Menu::onSettingsOption;
    lastTextScrollTime = millis();
    currentOptionLength = strlen(settings[settingsPointer.pointedOptionIndex]);
    displayVisibleOptions(settings, settingsPointer);
  }

  checkTextScrolling(scrolled, "Go back to settings", selectedOptPointer.displayLine);
}

void Menu::onAboutOption() {
  bool scrolled = checkOptionsScrolling(noAboutLines, aboutLines, selectedOptPointer);
  checkTextScrolling(scrolled, aboutLines[selectedOptPointer.pointedOptionIndex], selectedOptPointer.displayLine);

  if (InputManager::buttonWasClicked && selectedOptPointer.pointedOptionIndex == noAboutLines - 1) {
    currentUpdateAction = &Menu::onMainMenu;
    displayVisibleOptions(mainMenuOptions, mainMenuPointer);
  }
}

bool Menu::checkOptionsScrolling(uint8_t noOptions, const char** options, PointerPosition& pointer) {
  if (InputManager::newJoyDir) {
    if (InputManager::currentJoyDir == InputManager::JoyDirection::UP && pointer.pointedOptionIndex > 0) {
      pointer.displayLine = 0;
      pointer.pointedOptionIndex--;
      currentOptionLength = strlen(options[pointer.pointedOptionIndex]);
      displayVisibleOptions(options, pointer);
      return true;
    }
    if (InputManager::currentJoyDir == InputManager::JoyDirection::DOWN && pointer.pointedOptionIndex < noOptions - 1) {
      pointer.displayLine = 1;
      pointer.pointedOptionIndex++;
      currentOptionLength = strlen(options[pointer.pointedOptionIndex]);
      displayVisibleOptions(options, pointer);
      return true;
    }
  }
  return false;
}

void Menu::checkTextScrolling(bool justArrivedOnText, const char* text, uint8_t displayLine) {
  if (justArrivedOnText && currentOptionLength >= Renderer::lcdWidth) {
    scrollingTextIndex = 0;
    lastTextScrollTime = millis();
  }
  if (currentOptionLength >= Renderer::lcdWidth && millis() - lastTextScrollTime >= scrollingTextSpeed) {
    ++scrollingTextIndex;
    if (scrollingTextIndex == currentOptionLength - Renderer::lcdWidth + 2) {
      scrollingTextIndex = 0;
      Renderer::lcdController.setCursor(1, displayLine);
      Renderer::lcdController.print(text);
    }
    else {
      Renderer::lcdController.setCursor(1, displayLine);
      Renderer::lcdController.print(text + scrollingTextIndex);
    }
    lastTextScrollTime = millis();
  }
} 

void Menu::displayVisibleOptions(const char** options, PointerPosition& pointer) {
  Renderer::lcdController.clear();
  if (pointer.displayLine == 0) {
    Renderer::lcdController.print('>');
    Renderer::lcdController.print(options[pointer.pointedOptionIndex]);
    Renderer::lcdController.setCursor(1, 1);
    Renderer::lcdController.print(options[pointer.pointedOptionIndex + 1]);
  }
  else {
    Renderer::lcdController.setCursor(1, 0);
    Renderer::lcdController.print(options[pointer.pointedOptionIndex - 1]);
    Renderer::lcdController.setCursor(0, 1);
    Renderer::lcdController.print('>');
    Renderer::lcdController.print(options[pointer.pointedOptionIndex]);
  }
}
