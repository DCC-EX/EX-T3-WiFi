#include <UI.h>

UI::UI() { }

void UI::loop() { }

void UI::redraw() {
  auto redraw = [](auto& elements) {
    for (const auto& ptr : elements) {
      ptr->draw();
    }
  };

  if (_child != nullptr) {
    redraw(_child->_elements);
  } else {
    redraw(_elements);
    for (const auto& component : _components) {
      redraw(component->_elements);
    }
  }
}

bool UI::touch(uint8_t count, GTPoint* points) {
  auto touch = [&points](auto& elements) -> bool {
    for (const auto& ptr : elements) {
      auto touch = dynamic_cast<Touch*>(ptr.get());
      if (touch != nullptr && touch->contains(points[0].x, points[0].y)) {
        touch->touched(ptr.get());
        return true;
      }
    }
    return false;
  };

  if (_child != nullptr) {
    return touch(_child->_elements);
  }

  if (touch(_elements)) {
    return true;
  }
  for (const auto& component : _components) {
    if (touch(component->_elements)) {
      return true;
    }
  }

  return false;
}

bool UI::release(uint8_t count, GTPoint* points) {
  auto release = [&points](auto& elements) -> bool {
    for (const auto& ptr : elements) {
      auto touch = dynamic_cast<Touch*>(ptr.get());
      if (touch != nullptr && touch->contains(points[0].x, points[0].y)) {
        touch->released(ptr.get());
        return true;
      }
    }
    return false;
  };

  if (_child != nullptr) {
    return release(_child->_elements);
  }

  if (release(_elements)) {
    return true;
  }
  for (const auto& component : _components) {
    if (release(component->_elements)) {
      return true;
    }
  }
  return false;
}

bool UI::handleTouch(uint8_t count, GTPoint* points, std::function<bool()> touched) {
  if (touch(count, points)) {
    while (touched()) {
      delay(25);
    }
    return release(count, points);
  }
  return false;
}

void UI::encoderRotate(Encoder::Rotation rotation) {
  if (_child != nullptr) {
    _child->encoderRotate(rotation);
  } else {
    for (const auto& component : _components) {
      component->encoderRotate(rotation);
    }
  }
}

void UI::encoderPress(Encoder::ButtonPress press) {
  if (_child != nullptr) {
    _child->encoderPress(press);
  } else {
    for (const auto& component : _components) {
      component->encoderPress(press);
    }
  }
}

void UI::swipe(Swipe swipe) {
  if (_child != nullptr) {
    _child->swipe(swipe);
  } else {
    for (const auto& component : _components) {
      component->swipe(swipe);
    }
  }
}

void UI::reset(bool redrawAfter) {
  UI::tft->fillRect(0, 30, 320, 450, TFT_BLACK);
  _child.reset();
  if (redrawAfter) {
    redraw();
  }
}
