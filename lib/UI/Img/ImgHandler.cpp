#include <Img/ImgHandler.h>
#include <Img/BMP.h>

ImgHandler* ImgHandler::load(fs::File img) {
  char* ext = strchr(img.name(), '.');

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

ImgHandler::ImgHandler(fs::File img) : _img(img) { }

ImgHandler::ImgHandler(fs::File img, TFT_eSPI* tft, int16_t x, int16_t y) : ImgHandler(img) { }

ImgHandler::~ImgHandler() {
  _img.close();
}

ImgHandler::operator bool() const {
  return _img;
}
