#include <Elements/Header.h>
#include <UI.h>

Header::Header(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
               const String& label, uint16_t color)
    : Label(x, y, w, h, label, true, color) {
  draw();
}

void Header::draw() {
  Label::draw();
  
  uint16_t textW = UI::tft->textWidth(_label);
  uint16_t lineY = Element::_y + (UI::tft->fontHeight() / 2);
  uint16_t lineW = ((Element::_w - textW) / 2) - 10;

  UI::tft->fillRect(Element::_x, lineY, lineW, 2, _color);
  UI::tft->fillRect(Element::_w - lineW, lineY, lineW, 2, _color);
}
