#include <Paging.h>

Paging::Paging(uint8_t pages) : _pages(pages) {
  _elements.reserve(3);

  addElement<Button>(0, 435, 102, 42, "<",
    Button::Appearance {
      TFT_WHITE,
      TFT_DARKGREY,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_DARKGREY,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onRelease(std::bind(&Paging::prev, this));

  addElement<Button>(109, 435, 102, 42,
    Button::Appearance {
      TFT_WHITE,
      TFT_DARKGREY,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_DARKGREY,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onRelease(std::bind(&Paging::reset, this));

  addElement<Button>(218, 435, 102, 42, ">",
    Button::Appearance {
      TFT_WHITE,
      TFT_DARKGREY,
      TFT_WHITE
    },
    Button::Appearance {
      TFT_DARKGREY,
      TFT_WHITE,
      TFT_WHITE
    })
    ->onRelease(std::bind(&Paging::next, this));

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

  dispatchEvent(static_cast<uint8_t>(Event::CHANGED), &_page);
}

void Paging::encoderRotate(Encoder::Rotation rotation) {
  if (rotation == Encoder::Rotation::CW) {
    next();
  } else if (rotation == Encoder::Rotation::CCW) {
    prev();
  }
}

uint8_t Paging::getPage() {
  return _page;
}
