#include <Touch.h>

Touch::Touch(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t margin)
    : _x(x), _y(y), _w(w), _h(h), _margin(margin) { }

void Touch::onTouch(const EventCallback&& touched) {
  addEventListener(Event::TOUCH, std::move(touched));
}

void Touch::onRelease(const EventCallback&& released) {
  addEventListener(Event::RELEASE, std::move(released));
}

bool Touch::contains(uint16_t x, uint16_t y) {
  return ((x >= max(0, _x - _margin)) && (x <= min(320, _x + _w + _margin))
      && (y >= max(0, _y - _margin)) && (y <= min(480, _y + _h + _margin)));
}

void Touch::touched(void* parameter) {
  dispatchEvent(Event::TOUCH, parameter);
}

void Touch::released(void* parameter) {
  dispatchEvent(Event::RELEASE, parameter);
}
