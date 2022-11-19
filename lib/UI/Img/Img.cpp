#include <Img/Img.h>
#include <Img/BMP.h>

Img* Img::load(fs::File img) {
  char *ext = strchr(img.name(), '.');

  if (ext != NULL) {
    if (strcmp(ext, ".bmp") == 0) {
      return new BMP(img);
    } else if (strcmp(ext, ".jpg") == 0) {
      // TODO, future version?
    } else if (strcmp(ext, ".png") == 0) {
      // TODO, future version?
    }
  }

  return nullptr;
}

Img::Img(fs::File img) : _img(img) { }

Img::Img(fs::File img, TFT_eSPI *tft, int16_t x, int16_t y) : Img(img) { }

Img::~Img() {
  _img.close();
}

Img::operator bool() const {
  return _img;
}
