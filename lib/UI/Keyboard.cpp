#include <Keyboard.h>

Keyboard::Keyboard(const String& label, const String& value) {
  _elements.reserve(46);

  addElement<Header>(0, 50, 320, 18, label);

  _value = addElement<Input>(0, 78, 320, 30, value);

  addKeyRow(1, 120, row1Keys[0]);
  uint8_t row2Start = addKeyRow(1, 152, row2Keys[0]);
  uint8_t row3Start = addKeyRow(17, 184, row3Keys[0]);
  uint8_t row4Start = addKeyRow(49, 216, row4Keys[0]);

  _shiftKey = addElement<Button>(1, 216, 46, 30, "^", true);
  _specialKey = addElement<Button>(1, 248, 60, 30, "?!@", "ABC", true);
  
  // Shift - Toggle between upper and lower case
  _shiftKey->onRelease([this, row2Start, row3Start, row4Start](void* parameter) {
    _specialKey->setState(Button::State::IDLE);
    uint8_t set = _shiftKey->getState() == Button::State::PRESSED ? 1 : 0;
    changeKeyRow(row2Start, row2Keys[set]);
    changeKeyRow(row3Start, row3Keys[set]);
    changeKeyRow(row4Start, row4Keys[set]);
  });
  // Toggle ABC | Special Char
  _specialKey->onRelease([this, row2Start, row3Start, row4Start](void* parameter) {
    _shiftKey->setState(Button::State::IDLE);
    uint8_t set = _specialKey->getState() == Button::State::PRESSED ? 2 : 0;
    changeKeyRow(row2Start, row2Keys[set]);
    changeKeyRow(row3Start, row3Keys[set]);
    changeKeyRow(row4Start, row4Keys[set]);
  });
  // Backspace
  addElement<Button>(225 + 48, 216, 46, 30, "<")
    ->onRelease([this](void* parameter) {
      _value->backspace();
    });

  addElement<Button>(63, 248, 192, 30, " ")
    ->onRelease(std::bind(&Keyboard::addKeyToInput, this, std::placeholders::_1));
  addElement<Button>(257, 248, 30, 30, ",")
    ->onRelease(std::bind(&Keyboard::addKeyToInput, this, std::placeholders::_1));
  addElement<Button>(289, 248, 30, 30, ".")
    ->onRelease(std::bind(&Keyboard::addKeyToInput, this, std::placeholders::_1));

  addElement<Button>(0, 288, 157, 42, "Cancel",
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
    ->onRelease(std::bind(&Keyboard::dispatchEvent, this, Event::CANCEL, nullptr));

  addElement<Button>(163, 288, 157, 42, "Enter",
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
      String value = _value->getValue();
      dispatchEvent(Event::ENTER, &value);
    });
}

uint8_t Keyboard::addKeyRow(uint16_t x, uint16_t y, std::vector<char>& keys) {
  uint8_t start = _elements.size();

  for (char key : keys) {
    addElement<Button>(x, y, 30, 30, String(key))
      ->onRelease(std::bind(&Keyboard::addKeyToInput, this, std::placeholders::_1));
    x += 32;
  }

  return start;
}

void Keyboard::changeKeyRow(uint8_t rowStart, std::vector<char>& keys) {
  for (char key : keys) {
    (static_cast<Button*>(_elements[rowStart++].get()))->setLabel(String(key));
  }
}

void Keyboard::addKeyToInput(void* parameter) {
  Button* btn = static_cast<Button*>(parameter);
  _value->add(btn->getLabel().charAt(0));
}

String Keyboard::getValue() const {
  return _value->getValue();
}

void Keyboard::setValue(const String& value) {
  _value->setValue(value);
}
