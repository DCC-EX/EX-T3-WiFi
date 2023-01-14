#include <Paging.h>

Paging::Paging(uint8_t pages, uint16_t x, uint16_t y, uint16_t w) : _pages(pages) {
  _elements.reserve(3);

  w -= 14;
  w /= 3;

  auto idle = Button::Appearance {
    TFT_WHITE,
    TFT_DARKGREY,
    TFT_WHITE
  };
  auto pressed = Button::Appearance {
    TFT_DARKGREY,
    TFT_WHITE,
    TFT_WHITE
  };

  addElement<Button>(x, y, w, 42, "<", idle, pressed)
    ->onRelease([this](void*) {
      prev();
    });

  x += w + 7;
  addElement<Button>(x, y, w, 42, idle, pressed)
    ->onRelease([this](void*) {
      reset();
    });

  x += w + 7;
  addElement<Button>(x, y, w, 42, ">", idle, pressed)
    ->onRelease([this](void*) {
      next();
    });

  update();
}

void Paging::next() {
  _page++;
  if (_page > _pages) {
    _page = 1;
  }

  update();
}

void Paging::prev() {
  _page--;
  if (_page == 0) {
    _page = _pages;
  }

  update();
}

void Paging::reset() {
  _page = 1;

  update();
}

void Paging::update() {
  String label(_page);
  label += '\\';
  label += _pages;
  static_cast<Button*>(_elements[1].get())->setLabel(label);

  dispatchEvent(Event::CHANGED, &_page);
}

bool Paging::encoderRotate(Encoder::Rotation rotation) {
  if (rotation == Encoder::Rotation::CW) {
    next();
  } else if (rotation == Encoder::Rotation::CCW) {
    prev();
  }

  return true;
}

uint8_t Paging::getPage() {
  return _page;
}
