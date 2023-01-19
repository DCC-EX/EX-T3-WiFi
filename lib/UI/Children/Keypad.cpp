#include <Children/Keypad.h>
#include <algorithm>

Keypad::Keypad(const String& label, uint32_t max, uint32_t min, uint32_t* value)
    : _maxValue(std::min<uint32_t>(999999999, max)), _minValue(min) {
  _elements.reserve(17);

  addElement<Header>(0, 40, 320, 18, label);

  _number = addElement<Input>(0, 68, 320, 30, value != nullptr && isValid(*value) ? String(*value) : "");

  if (!_minValue && !_maxValue) {
    String digits("Max Digits - ");
    digits += MAX_DIGITS;
    addElement<Header>(0, 108, 320, 18, digits);
  } else {
    String range("Range ");
    range += _minValue;
    range += " - ";
    range += _maxValue;
    addElement<Header>(0, 108, 320, 18, range);
  }

  // Number buttons
  uint16_t x = 0;
  uint16_t y = 138;
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

  addElement<Button>(109, 282, 102, 40, _numberLabels[0])
    ->onTouch([this](void*) {
      keyPress(0);
    });

  addElement<Button>(0, 282, 102, 40, "Del",
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

  addElement<Button>(218, 282, 102, 40, "Clr",
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

  addElement<Button>(0, 328, 157, 42, "Cancel",
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

  addElement<Button>(163, 328, 157, 42, "Enter",
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
      uint32_t value = getNumber();
      if (isValid(value)) {
        dispatchEvent(Event::ENTER, &value);
      }
    });
}

Keypad::Keypad(const String& label, uint32_t max, uint32_t min) : Keypad(label, max, min, nullptr) { }

Keypad::Keypad(const String& label, uint32_t max, uint32_t min, uint32_t value) : Keypad(label, max, min, &value) { }

bool Keypad::isValid(uint32_t value) {
  return (!_minValue && !_maxValue) || (value >= _minValue && value <= _maxValue);
}

void Keypad::keyPress(uint8_t number) {
  if (_number->length() < MAX_DIGITS) {
    _number->add(_numberLabels[number][0]);
  }
}

uint32_t Keypad::getNumber() {
  return _number->getValue().toInt();
}

void Keypad::setNumber(uint32_t value) {
  if (isValid(value)) {
    _number->setValue(String(value));
  }
}
