#ifndef ELEMENT_H
#define ELEMENT_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class Element {
  protected:
    uint16_t _x, _y;
    uint16_t _w, _h;

    // TODO, move to separate class?
    void fillBorderRoundRect(uint32_t fill, uint32_t border, uint8_t radius = 4, uint8_t width = 2);
  public:
    Element(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    virtual ~Element() = default;
    virtual void draw() = 0;
};

#endif
