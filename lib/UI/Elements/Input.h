#ifndef INPUT_H
#define INPUT_H

#include <Elements/Element.h>

class Input : public Element {
  private:
    const uint16_t BG_COLOR = TFT_LIGHTGREY;
    String _text;
  protected:
    void drawText();
  public:
    Input(TFT_eSPI *tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    
    void draw();
    void add(const char c);
    void backspace();
    void clear();
    String get();
    void set(const String &str);
    uint8_t length();
};

#endif
