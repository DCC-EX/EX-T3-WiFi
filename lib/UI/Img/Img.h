#ifndef IMG_H
#define IMG_H

#include <FS.h>
#include <TFT_eSPI.h>

class Img {
  protected:
    fs::File _img;
  public:
    static Img* load(fs::File img);

    Img(fs::File img);
    Img(fs::File img, TFT_eSPI *tft, int16_t x, int16_t y);
    virtual ~Img();
    virtual bool dimensions(uint32_t &w, uint32_t &h) = 0;
    virtual bool draw(TFT_eSPI *tft, int16_t x, int16_t y) = 0;
    operator bool() const;
};

#endif
