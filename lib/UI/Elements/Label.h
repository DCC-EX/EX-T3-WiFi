#ifndef LABEL_H
#define LABEL_H

#include <Arduino.h>
#include <Elements/Element.h>

class Label : public Element {
  private:
    bool _center;
  protected:
    String _label;
    uint16_t _color;
  public:
    Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
          const String& label = "", bool center = false, uint16_t color = TFT_WHITE);

    inline Type getType() { return Type::Label; }
    void draw();
    void setLabel(const String& label, bool redraw = true);
    void setColor(uint16_t color, bool redraw = true);
};

#endif
