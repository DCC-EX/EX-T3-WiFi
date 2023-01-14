#include <Elements/Element.h>
#include <UI.h>

Element::Element(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : _x(x), _y(y), _w(w), _h(h), Touch(x, y, w, h, 1) { }

void Element::fillBorderRoundRect(uint32_t fill, uint32_t border, uint8_t radius, uint8_t width) {
  uint16_t x = _x;
  uint16_t y = _y;
  uint16_t w = _w;
  uint16_t h = _h;
  uint8_t r = min(w, h) / radius;

  if (fill != border && radius > 0) {
    UI::tft->fillRoundRect(x, y, w, h, r, border);
    x += width;
    y += width;
    w -= width * 2;
    h -= width * 2;
    r = min(w, h) / 4;
  }

  UI::tft->fillRoundRect(x, y, w, h, r, fill);
}
