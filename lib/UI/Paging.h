#ifndef PAGING_H
#define PAGING_H

#include <UI.h>
#include <Events.h>
#include <Elements/Button.h>

class Paging : public UI, public Events {
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

    Paging(uint8_t pages);
    
    void encoderRotate(Encoder::Rotation rotation);
    uint8_t getPage();
};

#endif
