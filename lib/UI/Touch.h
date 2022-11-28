#ifndef TOUCH_REGION_H
#define TOUCH_REGION_H

#include <Arduino.h>
#include <Events.h>

class Touch : Events {
  private:
    uint8_t _margin;
    uint16_t _x, _y;
    uint16_t _w, _h;
  public:
    enum class Event : uint8_t {
      TOUCH,
      RELEASE
    };

    Touch(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t margin = 0);

    void onTouch(const EventCallback&& touched);
    void onRelease(const EventCallback&& released);
    bool contains(uint16_t x, uint16_t y);
    void touched(void* parameter);
    void released(void* parameter);
};

#endif
