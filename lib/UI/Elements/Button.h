#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <SD.h>
#include <Element.h>

class Button : public Element {
  public:
    struct Appearance {
      struct Border {
        uint16_t color;
        uint8_t radius;
        uint8_t quadrants;

        Border(uint16_t color, uint8_t radius = 4, uint8_t quadrants = 0xF)
            : color(color), radius(radius), quadrants(quadrants) { }

        bool operator == (const Border& rhs) {
          return color == rhs.color && radius == rhs.radius && quadrants == rhs.quadrants;
        }

        bool operator != (const Border& rhs) {
          return !(*this == rhs);
        }
      };

      String label;
      String icon;
      uint16_t color;
      uint16_t fill;
      Border border;
      fs::FS& fs;

      Appearance(const String& label, uint16_t color, uint16_t fill, Border border, const String& icon = "", fs::FS& fs = SD)
          : label(label), color(color), fill(fill), border(border), icon(icon), fs(fs) { }
      Appearance(uint16_t color, uint16_t fill, Border border, const String& icon = "", fs::FS& fs = SD)
          : color(color), fill(fill), border(border), icon(icon), fs(fs) { }
      Appearance(const String& icon, fs::FS& fs = SD)
          : color(TFT_BLACK), fill(TFT_BLACK), border(TFT_BLACK), icon(icon), fs(fs) { }

      bool operator == (const Appearance& rhs) {
        return label == rhs.label && icon == rhs.icon && color == rhs.color && fill == rhs.fill && border == rhs.border;
      }

      bool operator != (const Appearance& rhs) {
        return !(*this == rhs);
      }
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
    Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
           Appearance idle, Appearance pressed,
           bool latching = false, State state = State::IDLE);

    Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String& label,
           bool latching = false, State state = State::IDLE);

    Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String& label, Appearance idle, Appearance pressed,
           bool latching = false, State state = State::IDLE);

    Button(int16_t x, int16_t y, uint16_t w, uint16_t h,
           const String& idleLabel, const String& pressedLabel,
           bool latching = false, State state = State::IDLE);

    Button(int16_t x, int16_t y, uint16_t w, uint16_t h, Appearance both);

    inline Type getType() { return Type::Button; }
    void draw();
    State getState();
    void setState(State state, bool redraw = true);
    String getLabel(State state = State::IDLE) const;
    void setLabel(const String& label, bool redraw = true);
    void setLabel(const String& label, State state, bool redraw = true);
    bool isLatching();
};

#endif
