#include <Elements/Button.h>
#include <Img/Img.h>

Button::Button(TFT_eSPI *tft,
               int16_t x, int16_t y, uint16_t w, uint16_t h,
               Appearance idle, Appearance pressed,
               bool latching, State state)
    : Element(tft, x, y, w, h), Touch(x, y, w, h), _idle(idle), _pressed(pressed), _latching(latching), _state(state) {
  draw();
}

Button::Button(TFT_eSPI *tft,
               int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String &label,
               bool latching, State state)
    : Button(tft, x, y, w, h, { label }, { label }, latching, state) { }

Button::Button(TFT_eSPI *tft,
               int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String &label, Appearance idle, Appearance pressed,
               bool latching, State state)
    : Button(tft, x, y, w, h,
             { label, idle.color, idle.fill, idle.border },
             { label, pressed.color, pressed.fill, pressed.border },
             latching, state) { }

Button::Button(TFT_eSPI *tft,
               int16_t x, int16_t y, uint16_t w, uint16_t h,
               const String &idleLabel, const String &pressedLabel,
               bool latching, State state)
    : Button(tft, x, y, w, h, { idleLabel, TFT_WHITE, TFT_BLACK, TFT_WHITE }, { pressedLabel, TFT_BLACK, TFT_WHITE, TFT_WHITE }, latching, state) { }

void Button::draw() {
  int16_t textX, textY;
  uint16_t textW, textH;

  Appearance &appearance = _state == State::PRESSED ? _pressed : _idle;

  textW = appearance.label.length() ? _tft->textWidth(appearance.label) : 0;
  textH = _tft->fontHeight();

  textX = (Element::_x + ((Element::_w - textW) / 2));
  textY = (Element::_y + ((Element::_h - textH) / 2)) + 2;

  fillBorderRoundRect(appearance.fill, appearance.border);

  if (appearance.icon.length()) {
    Img *img = Img::load(SD.open(appearance.icon));
    if (img != nullptr && *img) {
      uint32_t imgW, imgH;
      if (img->dimensions(imgW, imgH) &&
          img->draw(_tft, textX - (imgW / 2), Element::_y + ((Element::_h - imgH) / 2))) {
        // Only change `textX` if the icon was loaded successfully
        textX += (imgW / 2) + 2;
      }
    }
    delete img;
  }

  uint32_t oldTextColor = _tft->textcolor;
  uint32_t oldTextBGColor = _tft->textbgcolor;

  _tft->setCursor(textX, textY);
  _tft->setTextColor(appearance.color, appearance.fill);
  _tft->print(appearance.label);

  _tft->setTextColor(oldTextColor, oldTextBGColor);
}

void Button::touched() {
  setState(_latching ? (_state == State::IDLE ? State::PRESSED : State::IDLE) : State::PRESSED);
  Touch::touched(this);
}

void Button::released() {
  if (!_latching) {
    setState(State::IDLE);
  }
  Touch::released(this);
}

Button::State Button::getState() {
  return _state;
}

void Button::setState(State state) {
  if (state != _state) {
    _state = state;
    draw();
  }
}

String Button::getLabel(State state) const {
  return state == State::IDLE
    ? _idle.label
    : _pressed.label;
}

void Button::setLabel(const String &label) {
  _idle.label = label;
  _pressed.label = label;
  draw();
}

void Button::setLabel(const String &label, State state) {
  if (state == State::IDLE) {
    _idle.label = label;
  } else {
    _pressed.label = label;
  }

  draw();
}

bool Button::isLatching() {
  return _latching;
}
