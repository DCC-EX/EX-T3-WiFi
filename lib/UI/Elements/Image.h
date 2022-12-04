#ifndef IMAGE_H
#define IMAGE_H

#include <SPIFFS.h>
#include <SD.h>
#include <Elements/Element.h>

class Image : public Element {
  private:
    fs::FS& _fs;
    String _path;
  public:
    Image(uint16_t x, u_int16_t y, uint16_t w, uint16_t h, fs::FS& fs = SD);
    Image(uint16_t x, u_int16_t y, uint16_t w, uint16_t h, const String& path, fs::FS& fs = SD);
    
    void draw();
    void setImage(const String& path);
};

#endif
