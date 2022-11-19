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
    enum class Event : uint8_t {
      CHANGED
    };

    Paging(TFT_eSPI *tft, Tasks *tasks, uint8_t pages);
    
    void encoderRotate(Encoder::Rotation rotation);
    uint8_t getPage();
};

#endif
