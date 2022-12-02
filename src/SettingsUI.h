#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include <UI.h>
#include <Settings.h>

class SettingsUI : public UI {
  private:
    char _speedStepLabels[3][2] = { "1", "2", "4" };
    char _swipActionLabels[6][12] = { "Keypad", "Loco Names", "Loco Groups", "Next Loco", "Prev Loco", "Release" };
    #if defined(USE_ACCELEROMETER)
    char _rotationLabels[3][14] = { "Standard", "Inverted", "Accelerometer" };
    #else
    char _rotationLabels[2][9] = { "Standard", "Inverted" };
    #endif

    void swipeAction(void* button, uint8_t& gesture, uint8_t actions);
  public:
    SettingsUI();
    ~SettingsUI();
};

#endif
