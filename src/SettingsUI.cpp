#include <SettingsUI.h>
#include <Paging.h>
#include <Elements/Header.h>
#include <Elements/Label.h>
#include <Elements/Button.h>
#include <Settings.h>

SettingsUI::SettingsUI() : Page1(*this), Page2(*this) {
  _elements.reserve(20);

  addComponent<Paging>(_pages.size())
    ->addEventListener(Paging::Event::CHANGED, [this](void* parameter) {
      UI::tft->fillRect(0, 30, 320, 405, TFT_BLACK);
      _elements.clear();
      uint8_t page = *static_cast<uint8_t*>(parameter);
      _pages[page - 1]();
    });

  _pages[0]();
}

SettingsUI::~SettingsUI() {
  Settings.save();
}

void SettingsUI::Page1::swipeAction(void* button, uint8_t& gesture, uint8_t actions) {
  if (++gesture > actions) {
    gesture = 0;
  }

  static_cast<Button*>(button)->setLabel(_swipActionLabels[gesture]);
}

void SettingsUI::Page1::show() {
  _ui.addElement<Header>(0, 40, 320, 18, "Loco UI Options");

  _ui.addElement<Label>(0, 84, 130, 18, "Speed Step:");
  _ui.addElement<Button>(170, 70, 150, 42, _speedStepLabels[Settings.LocoUI.speedStep])
    ->onRelease([this](void* parameter) {
      if (++Settings.LocoUI.speedStep > 2) {
        Settings.LocoUI.speedStep = 0;
      }

      static_cast<Button*>(parameter)->setLabel(_speedStepLabels[Settings.LocoUI.speedStep]);
    });

  _ui.addElement<Label>(0, 133, 130, 18, "Swipe Up:");
  _ui.addElement<Button>(170, 119, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.up])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.up, 5);
    });

  _ui.addElement<Label>(0, 182, 130, 18, "Swipe Down:");
  _ui.addElement<Button>(170, 168, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.down])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.down, 5);
    });

  _ui.addElement<Label>(0, 231, 130, 18, "Swipe Left:");
  _ui.addElement<Button>(170, 217, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.left])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.left, 5);
    });

  _ui.addElement<Label>(0, 280, 130, 18, "Swipe Right:");
  _ui.addElement<Button>(170, 266, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.right])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.right, 5);
    });

  _ui.addElement<Label>(0, 329, 130, 18, "After Swipe Release:");
  _ui.addElement<Button>(170, 315, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.release])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.release, 4);
    });
}

void SettingsUI::Page2::show() {
  _ui.addElement<Header>(0, 40, 320, 18, "Screen Rotation");

  _ui.addElement<Button>(0, 70, 320, 42, _rotationLabels[Settings.rotation])
    ->onRelease([this](void* parameter) {
      #ifdef USE_ACCELEROMETER
      if (++Settings.rotation > 2) {
      #else
      if (++Settings.rotation > 1) {
      #endif
        Settings.rotation = 0;
      }

      static_cast<Button*>(parameter)->setLabel(_rotationLabels[Settings.rotation]);
      Settings.dispatchEvent(SettingsClass::Event::ROTATION_CHANGE);
    });
}
