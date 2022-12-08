#include <SettingsUI.h>
#include <Elements/Header.h>
#include <Elements/Label.h>
#include <Elements/Button.h>
#include <Settings.h>

SettingsUI::SettingsUI() {
  _elements.reserve(15);

  addElement<Header>(0, 40, 320, 18, "Loco UI Options");

  addElement<Label>(0, 84, 130, 18, "Speed Step:");
  addElement<Button>(170, 70, 150, 42, _speedStepLabels[Settings.LocoUI.speedStep])
    ->onRelease([this](void* parameter) {
      if (++Settings.LocoUI.speedStep > 2) {
        Settings.LocoUI.speedStep = 0;
      }

      static_cast<Button*>(parameter)->setLabel(_speedStepLabels[Settings.LocoUI.speedStep]);
    });

  addElement<Label>(0, 133, 130, 18, "Swipe Up:");
  addElement<Button>(170, 119, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.up])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, std::ref(Settings.LocoUI.Swipe.up), 5));

  addElement<Label>(0, 182, 130, 18, "Swipe Down:");
  addElement<Button>(170, 168, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.down])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, std::ref(Settings.LocoUI.Swipe.down), 5));

  addElement<Label>(0, 231, 130, 18, "Swipe Left:");
  addElement<Button>(170, 217, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.left])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, std::ref(Settings.LocoUI.Swipe.left), 5));

  addElement<Label>(0, 280, 130, 18, "Swipe Right:");
  addElement<Button>(170, 266, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.right])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, std::ref(Settings.LocoUI.Swipe.right), 5));

  addElement<Label>(0, 329, 130, 18, "After Swipe Release:");
  addElement<Button>(170, 315, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.release])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, std::ref(Settings.LocoUI.Swipe.release), 4));

  addElement<Header>(0, 369, 320, 18, "Screen Rotation");

  addElement<Button>(0, 399, 320, 42, _rotationLabels[Settings.rotation])
    ->onRelease([this](void* parameter) {
      #if defined(USE_ACCELEROMETER)
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

SettingsUI::~SettingsUI() {
  Settings.save();
}

void SettingsUI::swipeAction(void* button, uint8_t& gesture, uint8_t actions) {
  if (++gesture > actions) {
    gesture = 0;
  }

  static_cast<Button*>(button)->setLabel(_swipActionLabels[gesture]);
}
