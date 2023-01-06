#ifndef INPUT_H
#define INPUT_H

#include <Elements/Element.h>

class Input : public Element {
  private:
    const uint16_t BG_COLOR = TFT_LIGHTGREY;
    String _value;
  protected:
    void drawText(bool rect = true);
  public:
    Input(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const String& value = "");
    
    inline Type getType() { return Type::Input; }
    void draw();
    void add(const char c);
    void backspace();
    void clear();
    String getValue() const;
    void setValue(const String& value, bool redraw = true);
    uint8_t length();
};

#endif
