#pragma once

#include "constants.h"
#include "scene.h"

class Menu : Scene {
  private:
    using MenuAction = void (Menu::*)();
    struct PointerPosition {
      uint8_t pointedOptionIndex;
      uint8_t displayLine;
    };

  public:
    void start();
    void update();
    bool checkOptionsScrolling(uint8_t, const char**, PointerPosition&);
    void checkTextScrolling(bool, const char*, uint8_t);
    void displayVisibleOptions(const char**, PointerPosition&);
    void onMainMenu();
    void onAboutOption();
    void onHowToPlayOption();
    void onHighscoresOption();
    void onSettingsOption();
    void onEnterNameSetting();
    void onContrastSetting();
    void onLcdBrightSetting();
    void onMatrixBrightSetting();
    void onMuteSoundsSetting();
    Menu();
    ~Menu();

  private:
    MenuAction currentUpdateAction;
    PointerPosition mainMenuPointer;
    PointerPosition settingsPointer;
    PointerPosition selectedOptPointer;
    uint8_t currentOptionLength;
    uint8_t cursorCurrentColumn;
    uint8_t scrollingTextIndex;
    uint32_t lastTextScrollTime;
    char username[Constants::usernameSize];
    char** highscoresLines;
    uint8_t contrastValue;
    uint8_t lcdBrightValue;
    uint8_t matrixBrightValue;
    bool locked;
    const uint8_t unitsPerBlock;
    const uint16_t scrollingTextSpeed;
    const uint8_t noMainMenuOptions;
    const uint8_t noAboutLines;
    const uint8_t noHowToTextLines;
    const uint8_t noSettings;
    const char** mainMenuOptions;
    const char** aboutLines;
    const char** howToTextLines;
    const char** settings;
};