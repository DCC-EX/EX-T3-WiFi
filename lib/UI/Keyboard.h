#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <UI.h>
#include <Events.h>
#include <Elements/Header.h>
#include <Elements/Input.h>
#include <Elements/Button.h>

class Keyboard : public UI, public Events {
  private:
    Input* _value;

    Button* _shiftKey;
    Button* _specialKey;

    std::vector<std::vector<char>> row1Keys = {
      { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' }
    };
    std::vector<std::vector<char>> row2Keys = {
      { 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p' },
      { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P' },
      { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')' }
    };
    std::vector<std::vector<char>> row3Keys = {
      { 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' },
      { 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' },
      { '~', '[', ']', '-', '+', ':', ';', '"', '\'' }
    };
    std::vector<std::vector<char>> row4Keys = {
      { 'z', 'x', 'c', 'v', 'b', 'n', 'm' },
      { 'Z', 'X', 'C', 'V', 'B', 'N', 'M' },
      { '{', '}', '<', '>', '_', '?', '/' }
    };

    uint8_t addKeyRow(uint16_t x, uint16_t y, std::vector<char>& keys);
    void changeKeyRow(uint8_t rowStart, std::vector<char>& keys);
    void addKeyToInput(void* parameter);
  public:
    enum class Event : uint8_t {
      CANCEL,
      ENTER
    };

    Keyboard(const String& label, const String& value = "");

    String getValue() const;
    void setValue(const String& value);
};

#endif
