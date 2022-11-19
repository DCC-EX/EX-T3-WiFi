#include <Elements/Header.h>

Header::Header(TFT_eSPI *tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
               const String &label, uint16_t color)
    : Label(tft, x, y, w, h, label, true, color) {
  draw();
}

void Header::draw() {
  Label::draw();
  
  uint16_t textW = _tft->textWidth(_label);
  uint16_t lineY = _y + (_tft->fontHeight() / 2);
  uint16_t lineW = ((_w - textW) / 2) - 10;

  _tft->fillRect(_x, lineY, lineW, 2, _color);
  _tft->fillRect(_w - lineW, lineY, lineW, 2, _color);
}
