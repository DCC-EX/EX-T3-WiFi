#ifndef KEYPAD_H
#define KEYPAD_H

#include <UI.h>
#include <Events.h>
#include <Elements/Header.h>
#include <Elements/Input.h>
#include <Elements/Button.h>

class Keypad : public UI, public Events {
  private:
    const uint8_t MAX_DIGITS = 9;

    uint32_t _maxValue;
    uint32_t _minValue;
    
    const char _numberLabels[10][2] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

    Keypad(const String& label, uint32_t max, uint32_t min, uint32_t* value);

    bool isValid(uint32_t value);
    void keyPress(uint8_t number);
  protected:
    Input* _number;
  public:
    struct Event {
      static constexpr uint8_t CANCEL = 0;
      static constexpr uint8_t ENTER = 1;
    };

    Keypad(const String& label, uint32_t max = 0, uint32_t min = 0);
    Keypad(const String& label, uint32_t max, uint32_t min, uint32_t value);
    
    uint32_t getNumber();
    void setNumber(uint32_t value);
};

#endif
