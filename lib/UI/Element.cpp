#include <Element.h>
#include <UI.h>

Element::Element(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : _x(x), _y(y), _w(w), _h(h), Touch(x, y, w, h, 1) { }

void Element::fillBorderRoundRect(uint32_t fill, uint32_t border, uint8_t radius, uint8_t width, uint8_t quadrants) {
  uint16_t x = _x;
  uint16_t y = _y;
  uint16_t w = _w;
  uint16_t h = _h;
  uint8_t r = min(w, h) / radius;

  auto drawQuadrantRect = [&](uint32_t fg, uint32_t bg) {
    UI::tft->drawSmoothRoundRect(x, y, r, 1, w - 1, h - 1, fg, bg, quadrants);
    if ((quadrants & 0xC) == 0xC) { // Bottom
      UI::tft->fillRect(x, y, w, h - r + 1, fg);
    }
    if ((quadrants & 0x9) == 0x9) { // Left
      UI::tft->fillRect(x + r, y, w - r, h, fg);
    }
    if ((quadrants & 0x3) == 0x3) { // Top
      UI::tft->fillRect(x, y + r, w, h - r, fg);
    }
    if ((quadrants & 0x6) == 0x6) { // Right
      UI::tft->fillRect(x, y, w - r, h, fg);
    }
  };

  if (fill != border && width > 0) {
    if (quadrants == 0xF) {
      UI::tft->fillSmoothRoundRect(x, y, w, h, r, border);
    } else if (quadrants != 0) {
      drawQuadrantRect(border, TFT_BLACK);
    } else {
      UI::tft->fillRect(x, y, w, h, border);
    }
    
    x += width;
    y += width;
    w -= width * 2;
    h -= width * 2;
    r = min(w, h) / 4;
  }

  if (quadrants == 0xF) {
    UI::tft->fillSmoothRoundRect(x, y, w, h, r, fill);
  } else if (quadrants != 0) {
    drawQuadrantRect(fill, border);
  } else {
    UI::tft->fillRect(x, y, w, h, fill);
  }
}
