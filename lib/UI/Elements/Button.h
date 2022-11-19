#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <SD.h>
#include <Elements/Element.h>
#include <Touch.h>

class Button : public Element, public Touch {
  public:
    struct Appearance {
      public:
        String label;
        String icon;
        uint16_t color;
        uint16_t fill;
        uint16_t border;

        Appearance(const String &label, uint16_t color, uint16_t fill, uint16_t border, const String &icon = "")
            : label(label), color(color), fill(fill), border(border), icon(icon) { }
        Appearance(uint16_t color, uint16_t fill, uint16_t border)
            : color(color), fill(fill), border(border) { }
    };
    enum class State : uint8_t {
      IDLE,
      PRESSED
    };
  private:
    Appearance _idle;
    Appearance _pressed;

    bool _latching;
    State _state;
  public:
    Button(TFT_eSPI *tft,
           int16_t x, int16_t y, uint16_t w, uint16_t h,
           Appearance idle, Appearance pressed,
           bool latching = false, State state = State::IDLE);

    Button(TFT_eSPI *tft,
           int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String &label,
           bool latching = false, State state = State::IDLE);

    Button(TFT_eSPI *tft,
           int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String &label, Appearance idle, Appearance pressed,
           bool latching = false, State state = State::IDLE);

    Button(TFT_eSPI *tft,
           int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String &idleLabel, const String &pressedLabel,
           bool latching = false, State state = State::IDLE);

    void draw();
    void touched();
    void released();
    State getState();
    void setState(State state);
    String getLabel(State state = State::IDLE) const;
    void setLabel(const String &label);
    void setLabel(const String &label, State state);
    bool isLatching();
};

#endif
