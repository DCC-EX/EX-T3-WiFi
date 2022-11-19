#include <Elements/Label.h>

Label::Label(TFT_eSPI *tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
             const String &label, bool center, uint16_t color)
    : Element(tft, x, y, w, h), _center(center), _color(color) {
  // TODO, reserve label space?
  setLabel(label);
}

void Label::draw() {
  uint32_t oldTextColor = _tft->textcolor;
  uint32_t oldTextBGColor = _tft->textbgcolor;

  _tft->fillRect(_x, _y, _w, _h, TFT_BLACK);
  _tft->setTextColor(_color, TFT_BLACK);
  if (_center) {
    uint8_t datum = _tft->getTextDatum();
    _tft->setTextDatum(MC_DATUM);
    _tft->drawString(_label, _x + (_w / 2), _y + (_h / 2));
    _tft->setTextDatum(datum);
  } else {
    _tft->setCursor(_x, _y);
    _tft->print(_label);
  }

  _tft->setTextColor(oldTextColor, oldTextBGColor);
}

void Label::setLabel(const String &label) {
  _tft->fillRect(_x, _y, _w, _h, TFT_BLACK);
  _label = label;
  draw();
}

void Label::setColor(uint16_t color) {
  _color = color;
  draw();
}
