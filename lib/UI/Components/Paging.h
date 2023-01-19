#ifndef PAGING_H
#define PAGING_H

#include <Component.h>
#include <Events.h>
#include <Elements/Button.h>

class Paging : public Component, public Events {
  private:
    uint8_t _pages;
    uint8_t _page = 1;

    void next();
    void prev();
    void reset();
    void update();
  public:
    struct Event {
      static constexpr uint8_t CHANGED = 0;
    };

    Paging(uint8_t pages, uint16_t x = 0, uint16_t y = 435, uint16_t w = 320);
    
    bool encoderRotate(Encoder::Rotation rotation);
    uint8_t getPage();
};

#endif
