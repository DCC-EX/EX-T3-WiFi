#include <SettingsUI.h>
#include <Settings.h>
#include <Components/Paging.h>
#include <Children/Keypad.h>
#include <Elements/Header.h>
#include <Elements/Label.h>
#include <Elements/Button.h>

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
      swipeAction(parameter, Settings.LocoUI.Swipe.up, 6);
    });

  _ui.addElement<Label>(0, 182, 130, 18, "Swipe Down:");
  _ui.addElement<Button>(170, 168, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.down])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.down, 6);
    });

  _ui.addElement<Label>(0, 231, 130, 18, "Swipe Left:");
  _ui.addElement<Button>(170, 217, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.left])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.left, 6);
    });

  _ui.addElement<Label>(0, 280, 130, 18, "Swipe Right:");
  _ui.addElement<Button>(170, 266, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.right])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.right, 6);
    });

  _ui.addElement<Label>(0, 329, 130, 18, "After Swipe Release:");
  _ui.addElement<Button>(170, 315, 150, 42, _swipActionLabels[Settings.LocoUI.Swipe.release])
    ->onRelease([this](void* parameter) {
      swipeAction(parameter, Settings.LocoUI.Swipe.release, 5);
    });
}

void SettingsUI::Page2::show() {
  _ui.addElement<Header>(0, 40, 320, 18, "Screen Rotation");

  _ui.addElement<Button>(0, 70, 320, 42, _rotationLabels[Settings.rotation])
    ->onRelease([this](void* parameter) {
      if (++Settings.rotation > 2) {
        Settings.rotation = 0;
      }

      static_cast<Button*>(parameter)->setLabel(_rotationLabels[Settings.rotation]);
      Settings.dispatchEvent(SettingsClass::Event::ROTATION_CHANGE);
    });

  _ui.addElement<Header>(0, 124, 320, 18, "Pin Protect (For WiFi and Settings)");

  _ui.addElement<Button>(0, 154, 320, 42, "Not Set", "Pin Set", false, Settings.pin == 0 ? Button::State::IDLE : Button::State::PRESSED)
    ->onRelease([this](void* parameter) {
      auto keypad = _ui.setChild<Keypad>("Set Pin");
      keypad->addEventListener(Keypad::Event::ENTER, [this, btn = static_cast<Button*>(parameter)](void* parameter) {
        Settings.pin = *static_cast<uint32_t*>(parameter);
        btn->setState(Settings.pin == 0 ? Button::State::IDLE : Button::State::PRESSED, false);
        _ui._tasks.push_back([this] {
          _ui.reset(true);
        });
      });
      keypad->addEventListener(Keypad::Event::CANCEL, [this, btn = static_cast<Button*>(parameter)](void*) {
        btn->setState(Settings.pin == 0 ? Button::State::IDLE : Button::State::PRESSED, false);
        _ui._tasks.push_back([this] {
          _ui.reset(true);
        });
      });
    });

    _ui.addElement<Header>(0, 208, 320, 18, "Emergency Stop Trigger Hold Duration");

    _ui.addElement<Button>(0, 238, 320, 42, String(Settings.emergencyStop))
    ->onRelease([this](void* parameter) {
      auto duration = _ui.setChild<Keypad>("Set Duration", 5000, 1000);
      duration->addEventListener(Keypad::Event::ENTER, [this, btn = static_cast<Button*>(parameter)](void* parameter) {
        Settings.emergencyStop = *static_cast<uint16_t*>(parameter);
        btn->setLabel(String(Settings.emergencyStop));
        _ui._tasks.push_back([this] {
          _ui.reset(true);
        });
      });
      duration->addEventListener(Keypad::Event::CANCEL, [this, btn = static_cast<Button*>(parameter)](void*) {
        _ui._tasks.push_back([this] {
          _ui.reset(true);
        });
      });
    });
}
