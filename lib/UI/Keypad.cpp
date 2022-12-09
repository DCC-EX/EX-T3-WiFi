#include <Keypad.h>

Keypad::Keypad(const String& label, uint16_t max, uint16_t min, uint16_t* value) : _maxValue(max), _minValue(min) {
  _elements.reserve(17);

  addElement<Header>(0, 50, 320, 18, label);

  _number = addElement<Input>(0, 78, 320, 30, value == nullptr ? "" : String(*value));

  String range("Range ");
  range += min;
  range += " - ";
  range += max;
  addElement<Header>(0, 118, 320, 18, range);

  addElement<Button>(0, 150, 157, 42, "Cancel",
    Button::Appearance {
      TFT_WHITE,
      TFT_DARKGREY,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_DARKGREY,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onRelease([this](void*) {
      dispatchEvent(Event::CANCEL);
    });

  addElement<Button>(163, 150, 157, 42, "Enter",
    Button::Appearance {
      TFT_WHITE,
      TFT_DARKGREEN,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_DARKGREEN,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onRelease([this](void* parameter) {
      uint32_t valid = getNumber();
      if (valid >= _minValue && valid <= _maxValue) {
        dispatchEvent(Event::ENTER, &valid);
      }
    });

  // Number buttons
  uint16_t x = 0;
  uint16_t y = 198;
  for (uint8_t i = 1; i < 10; i++) {
    addElement<Button>(x, y, 102, 42, _numberLabels[i])
      ->onTouch([this, i](void*) {
        keyPress(i);
      });
    if (i % 3 == 0) { // New row
      x = 0;
      y += 48;
    } else {
      x += 109;
    }
  }

  addElement<Button>(109, 342, 102, 40, _numberLabels[0])
    ->onTouch([this](void*) {
      keyPress(0);
    });

  addElement<Button>(0, 342, 102, 40, "Del",
    Button::Appearance {
      TFT_WHITE,
      TFT_ORANGE,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_ORANGE,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onTouch([this](void*) {
      _number->backspace();
    });

  addElement<Button>(218, 342, 102, 40, "Clr",
    Button::Appearance {
      TFT_WHITE,
      TFT_ORANGE,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_ORANGE,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onTouch([this](void*) {
      _number->clear();
    });
}

Keypad::Keypad(const String& label, uint16_t max, uint16_t min) : Keypad(label, max, min, nullptr) { }

Keypad::Keypad(const String& label, uint16_t max, uint16_t min, uint16_t value) : Keypad(label, max, min, &value) { }

void Keypad::keyPress(uint8_t number) {
  if (_number->length() < MAX_DIGITS - 1) {
    _number->add(_numberLabels[number][0]);
  }
}

uint32_t Keypad::getNumber() {
  return _number->getValue().toInt();
}

void Keypad::setNumber(uint32_t value) {
  _number->setValue(String(value));
}
