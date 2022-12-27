#ifndef SETTINGS_UI_H
#define SETTINGS_UI_H

#include <UI.h>
#include <Settings.h>

class SettingsUI : public UI {
  private:
    class Page {
      protected:
        SettingsUI& _ui;
      public:
        Page(SettingsUI& ui) : _ui(ui) { }

        virtual void show() = 0;
    };

    class Page1 : Page {
      using Page::Page;

      char _speedStepLabels[3][2] = { "1", "2", "4" };
      char _swipActionLabels[6][12] = { "Keypad", "Loco Names", "Loco Groups", "Next Loco", "Prev Loco", "Release" };

      void swipeAction(void* button, uint8_t& gesture, uint8_t actions);
    public:
      void show();
    } Page1;

    class Page2 : Page {
      using Page::Page;

      #ifdef USE_ACCELEROMETER
      char _rotationLabels[3][14] = { "Standard", "Inverted", "Accelerometer" };
      #else
      char _rotationLabels[2][9] = { "Standard", "Inverted" };
      #endif
    public:
      void show();
    } Page2;

    std::vector<std::function<void (void)>> _pages = {
      [this] { Page1.show(); },
      [this] { Page2.show(); }
    };
  public:
    SettingsUI();
    ~SettingsUI();
};

#endif
