#ifndef KEYPAD_H
#define KEYPAD_H

#include <UI.h>
#include <Events.h>
#include <Elements/Header.h>
#include <Elements/Input.h>
#include <Elements/Button.h>

class Keypad : public UI, public Events {
  private:
    const uint8_t MAX_DIGITS = 6;

    uint16_t _maxValue;
    uint16_t _minValue;
    Input *_number;

    const char _numberLabels[10][2] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

    void keyPress(uint8_t number);
  public:
    enum class Event : uint8_t {
      CANCEL,
      ENTER
    };

    Keypad(TFT_eSPI *tft, Tasks *tasks, const char *label, uint16_t max, uint16_t min = 0);
    
    uint32_t getNumber();
};

#endif
