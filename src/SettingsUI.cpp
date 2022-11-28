#include <SettingsUI.h>
#include <Elements/Header.h>
#include <Elements/Label.h>
#include <Elements/Button.h>
#include <Settings.h>

SettingsUI::SettingsUI() {
  _elements.reserve(15);

  addElement<Header>(0, 40, 320, 18, "Loco UI Options");

  addElement<Label>(0, 84, 130, 18, "Speed Step:");
  addElement<Button>(170, 70, 150, 42, _speedStepLabels[static_cast<uint8_t>(Settings.LocoUI.speedStep)])
    ->onRelease(std::bind(&SettingsUI::speedStep, this, std::placeholders::_1, &Settings.LocoUI.speedStep));

  addElement<Label>(0, 133, 130, 18, "Swipe Up:");
  addElement<Button>(170, 119, 150, 42, _swipActionLabels[static_cast<uint8_t>(Settings.LocoUI.Swipe.up)])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, &Settings.LocoUI.Swipe.up, 5));

  addElement<Label>(0, 182, 130, 18, "Swipe Down:");
  addElement<Button>(170, 168, 150, 42, _swipActionLabels[static_cast<uint8_t>(Settings.LocoUI.Swipe.down)])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, &Settings.LocoUI.Swipe.down, 5));

  addElement<Label>(0, 231, 130, 18, "Swipe Left:");
  addElement<Button>(170, 217, 150, 42, _swipActionLabels[static_cast<uint8_t>(Settings.LocoUI.Swipe.left)])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, &Settings.LocoUI.Swipe.left, 5));

  addElement<Label>(0, 280, 130, 18, "Swipe Right:");
  addElement<Button>(170, 266, 150, 42, _swipActionLabels[static_cast<uint8_t>(Settings.LocoUI.Swipe.right)])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, &Settings.LocoUI.Swipe.right, 5));

  addElement<Label>(0, 329, 130, 18, "After Swipe Release:");
  addElement<Button>(170, 315, 150, 42, _swipActionLabels[static_cast<uint8_t>(Settings.LocoUI.Swipe.release)])
    ->onRelease(std::bind(&SettingsUI::swipeAction, this, std::placeholders::_1, &Settings.LocoUI.Swipe.release, 4));

  addElement<Header>(0, 369, 320, 18, "Screen Rotation");

  addElement<Button>(0, 399, 320, 42, _rotationLabels[static_cast<uint8_t>(Settings.rotation)])
    ->onRelease([this](void* parameter) {
      uint8_t rotation = static_cast<uint8_t>(Settings.rotation);
      rotation++;

      #if defined(USE_ACCELEROMETER)
      if (rotation > 2) {
      #else
      if (rotation > 1) {
      #endif
        rotation = 0;
      }

      Settings.rotation = static_cast<SettingsClass::Rotation>(rotation);
      static_cast<Button*>(parameter)->setLabel(_rotationLabels[rotation]);

      Settings.dispatchEvent(static_cast<uint8_t>(SettingsClass::Event::ROTATION_CHANGE));
    });
}

SettingsUI::~SettingsUI() {
  Settings.save();
}

void SettingsUI::speedStep(void* button, SettingsClass::LocoUI::SpeedStep* speedStep) {
  uint8_t step = static_cast<uint8_t>(*speedStep);

  if (++step > 2) {
    step = 0;
  }

  *speedStep = static_cast<SettingsClass::LocoUI::SpeedStep>(step);
  static_cast<Button*>(button)->setLabel(_speedStepLabels[step]);
}

void SettingsUI::swipeAction(void* button, SettingsClass::LocoUI::Swipe::Action* gesture, uint8_t actions) {
  uint8_t action = static_cast<uint8_t>(*gesture);

  if (++action > actions) {
    action = 0;
  }

  *gesture = static_cast<SettingsClass::LocoUI::Swipe::Action>(action);
  static_cast<Button*>(button)->setLabel(_swipActionLabels[action]);
}
