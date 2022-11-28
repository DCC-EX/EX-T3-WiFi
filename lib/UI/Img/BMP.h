#ifndef BMP_H
#define BMP_H

#include <Img/ImgHandler.h>

// BMP code based of Bodmers example
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Generic/TFT_SPIFFS_BMP/BMP_functions.ino

class BMP : public ImgHandler {
  private:
    uint16_t read16();
    uint32_t read32();
  public:
    BMP(fs::File bmp);
    BMP(fs::File bmp, TFT_eSPI* tft, int16_t x, int16_t y);
    bool dimensions(uint32_t& w, uint32_t& h);
    bool draw(TFT_eSPI* tft, int16_t x, int16_t y);
};

#endif
