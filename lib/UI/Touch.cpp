#include <Touch.h>

Touch::Touch(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    : _x(x), _y(y), _w(w), _h(h) { }

void Touch::onTouch(const EventCallback &&touched) {
  addEventListener(static_cast<uint8_t>(Event::TOUCH), std::move(touched));
}

void Touch::onRelease(const EventCallback &&released) {
  addEventListener(static_cast<uint8_t>(Event::RELEASE), std::move(released));
}

bool Touch::contains(uint16_t x, uint16_t y) {
  return ((x >= _x) && (x <= (_x + _w)) && (y >= _y) && (y <= (_y + _h)));
}

void Touch::touched(void *parameter) {
  dispatchEvent(static_cast<uint8_t>(Event::TOUCH), parameter);
}

void Touch::released(void *parameter) {
  dispatchEvent(static_cast<uint8_t>(Event::RELEASE), parameter);
}
