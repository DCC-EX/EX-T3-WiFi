#ifndef HEADER_H
#define HEADER_H

#include <Elements/Label.h>

class Header : public Label {
  public:
    Header(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
           const String& label, uint16_t color = TFT_WHITE);

    inline Type getType() { return Type::Header; }
    void draw();
};

#endif
