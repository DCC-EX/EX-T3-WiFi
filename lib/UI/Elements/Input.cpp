#include <Elements/Input.h>

Input::Input(TFT_eSPI *tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : Element(tft, x, y, w, h) {
  _text.reserve(20);
  draw();
}

void Input::draw() {
  fillBorderRoundRect(BG_COLOR, TFT_WHITE);
  if (_text.length()) {
    drawText();
  }
}

void Input::drawText() {
  uint8_t r = min(_w - 4, _h - 4) / 4;
  _tft->fillRoundRect(_x + 2, _y + 2, _w - 4, _h - 4, r, BG_COLOR);

  uint32_t oldTextColor = _tft->textcolor;
  uint32_t oldTextBGColor = _tft->textbgcolor;

  uint16_t textH =  _tft->fontHeight();
  uint16_t textY = (_y + ((_h - textH) / 2)) + 2;
  _tft->setCursor(10, textY);
  _tft->setTextColor(TFT_BLACK, BG_COLOR);
  _tft->print(_text);

  _tft->setTextColor(oldTextColor, oldTextBGColor);
}

void Input::add(const char c) {
  _text += c;
  drawText();
}

void Input::backspace() {
  if (_text.length()) {
    _text.remove(_text.length() - 1);
    drawText();
  }
}

void Input::clear() {
  _text.clear();
  drawText();
}

String Input::get() {
  return _text;
}

void Input::set(const String &str) {
  _text = str;
  drawText();
}

uint8_t Input::length() {
  return _text.length();
}
