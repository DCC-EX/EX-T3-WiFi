#include <Elements/Button.h>
#include <UI.h>
#include <Img/ImgHandler.h>

Button::Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
               Appearance idle, Appearance pressed,
               bool latching, State state)
    : Element(x, y, w, h), Touch(x, y, w, h, 1), _idle(idle), _pressed(pressed), _latching(latching), _state(state) {
  draw();

  onTouch([this](void*) {
    setState(_latching ? (_state == State::IDLE ? State::PRESSED : State::IDLE) : State::PRESSED);
  });
  onRelease([this](void*) {
    if (!_latching) {
      setState(State::IDLE);
    }
  });
}

Button::Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String& label,
               bool latching, State state)
    : Button(x, y, w, h, { label }, { label }, latching, state) { }

Button::Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String& label, Appearance idle, Appearance pressed,
               bool latching, State state)
    : Button(x, y, w, h,
             { label, idle.color, idle.fill, idle.border },
             { label, pressed.color, pressed.fill, pressed.border },
             latching, state) { }

Button::Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String& idleLabel, const String& pressedLabel,
               bool latching, State state)
    : Button(x, y, w, h, { idleLabel, TFT_WHITE, TFT_BLACK, TFT_WHITE }, { pressedLabel, TFT_BLACK, TFT_WHITE, TFT_WHITE }, latching, state) { }

Button::Button(int16_t x, int16_t y, uint16_t w, uint16_t h, Appearance both)
    : Button(x, y, w, h, both, both, false, State::IDLE) { }

void Button::draw() {
  int16_t textX, textY;
  uint16_t textW, textH;

  Appearance &appearance = _state == State::PRESSED ? _pressed : _idle;

  textW = appearance.label.length() ? UI::tft->textWidth(appearance.label) : 0;
  textH = UI::tft->fontHeight();

  textX = (Element::_x + ((Element::_w - textW) / 2));
  textY = (Element::_y + ((Element::_h - textH) / 2)) + 2;

  fillBorderRoundRect(appearance.fill, appearance.border);

  if (appearance.icon.length()) {
    ImgHandler* img = ImgHandler::load(appearance.fs.open(appearance.icon));
    if (img != nullptr && *img) {
      uint32_t imgW, imgH;
      if (img->dimensions(imgW, imgH) &&
          img->draw(UI::tft, textX - (imgW / 2), Element::_y + ((Element::_h - imgH) / 2))) {
        // Only change `textX` if the icon was loaded successfully
        textX += (imgW / 2) + 2;
      }
    }
    delete img;
  }

  uint32_t oldTextColor = UI::tft->textcolor;
  uint32_t oldTextBGColor = UI::tft->textbgcolor;

  UI::tft->setCursor(textX, textY);
  UI::tft->setTextColor(appearance.color, appearance.fill);
  UI::tft->print(appearance.label);

  UI::tft->setTextColor(oldTextColor, oldTextBGColor);
}

Button::State Button::getState() {
  return _state;
}

void Button::setState(State state) {
  if (state != _state && _idle != _pressed) {
    _state = state;
    draw();
  }
}

String Button::getLabel(State state) const {
  return state == State::IDLE
    ? _idle.label
    : _pressed.label;
}

void Button::setLabel(const String& label) {
  if (_idle.label != label || _pressed.label != label) {
    _idle.label = label;
    _pressed.label = label;
    draw();
  }
}

void Button::setLabel(const String& label, State state) {
  if (state == State::IDLE && _idle.label != label) {
    _idle.label = label;
    draw();
  } else if (state == State::PRESSED && _pressed.label != label) {
    _pressed.label = label;
    draw();
  }
}

bool Button::isLatching() {
  return _latching;
}
