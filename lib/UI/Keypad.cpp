#include <Keypad.h>

Keypad::Keypad(TFT_eSPI *tft, Tasks *tasks, const char *label, uint16_t max, uint16_t min)
    : UI(tft, tasks), _maxValue(max), _minValue(min) {
  _elements.reserve(17);

  addElement<Header>(0, 50, 320, 18, label);

  _number = addElement<Input>(0, 78, 320, 30);

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
    ->onRelease(std::bind(&Keypad::dispatchEvent, this, static_cast<uint8_t>(Event::CANCEL), nullptr));

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
    ->onRelease([this](void *parameter) {
      uint32_t valid = getNumber();
      if (valid >= _minValue && valid <= _maxValue) {
        dispatchEvent(static_cast<uint8_t>(Event::ENTER), &valid);
      }
    });

  // Number buttons
  uint16_t x = 0;
  uint16_t y = 198;
  for (uint8_t i = 1; i < 10; i++) {
    addElement<Button>(x, y, 102, 42, _numberLabels[i])
      ->onTouch(std::bind(&Keypad::keyPress, this, i));
    if (i % 3 == 0) { // New row
      x = 0;
      y += 48;
    } else {
      x += 109;
    }
  }

  addElement<Button>(109, 342, 102, 40, _numberLabels[0])
    ->onTouch(std::bind(&Keypad::keyPress, this, 0));

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
    ->onTouch(std::bind(&Input::backspace, _number));

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
    ->onTouch(std::bind(&Input::clear, _number));
}

void Keypad::keyPress(uint8_t number) {
  if (_number->length() < MAX_DIGITS - 1) {
    _number->add(_numberLabels[number][0]);
  }
}

uint32_t Keypad::getNumber() {
  return _number->get().toInt();
}
