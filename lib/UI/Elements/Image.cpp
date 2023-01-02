#include <Elements/Image.h>
#include <UI.h>
#include <Img/ImgHandler.h>

Image::Image(uint16_t x, u_int16_t y, uint16_t w, uint16_t h, fs::FS& fs)
    : Element(x, y, w, h), _fs(fs) { }

Image::Image(uint16_t x, u_int16_t y, uint16_t w, uint16_t h, const String& path, fs::FS& fs)
    : Element(x, y, w, h), _path(path), _fs(fs) {
  draw();
}

void Image::draw() {
  ImgHandler* img = ImgHandler::load(_fs.open(_path));
  if (img != nullptr && *img) {
    img->draw(UI::tft, _x, _y);
  }
  delete img;
}

void Image::setImage(const String& path, bool redraw) {
  if (_path != path) {
    _path = path;
    if (redraw) {
      draw();
    }
  }
}
