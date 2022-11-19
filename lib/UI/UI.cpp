#include <UI.h>

UI::UI(TFT_eSPI *tft, Tasks *tasks) : _tft(tft), _tasks(tasks) { }

void UI::loop() { }

void UI::rotated() {
  for (const auto &ptr : _elements) {
    ptr->draw();
  }
}

void UI::touch(uint8_t count, GTPoint* points) {
  for (const auto &ptr : _elements) {
    auto button = dynamic_cast<Button*>(ptr.get());
    if (button != nullptr && button->contains(points[0].x, points[0].y)) {
      button->touched();
      break;
    }
  }
}

void UI::release(uint8_t count, GTPoint* points) {
  for (const auto &ptr : _elements) {
    auto button = dynamic_cast<Button*>(ptr.get());
    if (button != nullptr && button->contains(points[0].x, points[0].y)) {
      button->released();
      break;
    }
  }
}

void UI::encoderRotate(Encoder::Rotation rotation) { }

void UI::encoderPress(Encoder::ButtonPress press) { }

void UI::swipe(Swipe swipe) { }
