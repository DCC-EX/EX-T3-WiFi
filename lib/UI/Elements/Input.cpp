#include <Elements/Input.h>
#include <UI.h>

Input::Input(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String& value) : Element(x, y, w, h) {
  // _value.reserve(20);
  _value = value;
  draw();
}

void Input::draw() {
  fillBorderRoundRect(BG_COLOR, TFT_WHITE);
  if (_value.length()) {
    drawText();
  }
}

void Input::drawText(bool rect) {
  if (rect) {
    uint8_t r = min(_w - 4, _h - 4) / 4;
    UI::tft->fillRoundRect(_x + 2, _y + 2, _w - 4, _h - 4, r, BG_COLOR);
  }

  uint32_t oldTextColor = UI::tft->textcolor;
  uint32_t oldTextBGColor = UI::tft->textbgcolor;

  uint16_t textH =  UI::tft->fontHeight();
  uint16_t textY = (_y + ((_h - textH) / 2)) + 2;
  UI::tft->setCursor(_x + 10, textY);
  UI::tft->setTextColor(TFT_BLACK, BG_COLOR);
  UI::tft->print(_value);

  UI::tft->setTextColor(oldTextColor, oldTextBGColor);
}

void Input::add(const char c) {
  _value += c;
  drawText(false);
}

void Input::backspace() {
  if (_value.length()) {
    _value.remove(_value.length() - 1);
    uint16_t textW = UI::tft->textWidth(_value);

    UI::tft->fillRect(_x + 10 + textW, _y + 2, _w - 14 - textW, _h - 4, BG_COLOR);
  }
}

void Input::clear() {
  _value.clear();
  drawText();
}

String Input::getValue() const {
  return _value;
}

void Input::setValue(const String& value, bool redraw) {
  _value = value;
  if (redraw) {
    drawText();
  }
}

uint8_t Input::length() {
  return _value.length();
}
