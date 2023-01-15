#ifndef PIN_H
#define PIN_H

#include <Keypad.h>

class Pin : public Keypad {
  private:
    uint32_t _pin;
  public:
    struct Event : Keypad::Event {
      static constexpr uint8_t VALID = 2;
    };

    Pin(uint32_t pin);
};

#endif
