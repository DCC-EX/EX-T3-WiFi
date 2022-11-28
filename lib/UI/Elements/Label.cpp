#include <Elements/Label.h>
#include <UI.h>

Label::Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
             const String& label, bool center, uint16_t color)
    : Element(x, y, w, h), Touch(x, y, w, h, 1), _center(center), _color(color) {
  // TODO, reserve label space?
  setLabel(label);
}

void Label::draw() {
  uint32_t oldTextColor = UI::tft->textcolor;
  uint32_t oldTextBGColor = UI::tft->textbgcolor;

  UI::tft->fillRect(Element::_x, Element::_y, Element::_w, Element::_h, TFT_BLACK);
  UI::tft->setTextColor(_color, TFT_BLACK);
  if (_center) {
    uint8_t datum = UI::tft->getTextDatum();
    UI::tft->setTextDatum(MC_DATUM);
    UI::tft->drawString(_label, Element::_x + (Element::_w / 2), Element::_y + (Element::_h / 2));
    UI::tft->setTextDatum(datum);
  } else {
    UI::tft->setCursor(Element::_x, Element::_y);
    UI::tft->print(_label);
  }

  UI::tft->setTextColor(oldTextColor, oldTextBGColor);
}

void Label::setLabel(const String& label) {
  if (_label != label) {
    UI::tft->fillRect(Element::_x, Element::_y, Element::_w, Element::_h, TFT_BLACK);
    _label = label;
    draw();
  }
}

void Label::setColor(uint16_t color) {
  if (_color != color) {
    _color = color;
    draw();
  }
}
