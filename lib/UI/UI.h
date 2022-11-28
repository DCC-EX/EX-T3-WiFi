#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <GT911.h>
#include <Elements/Element.h>
#include <Elements/Button.h>

#include <vector>
#include <deque>

class UI {
  public:
    static inline TFT_eSPI* tft;
    static inline std::deque<std::function<void (void)>> tasks;
  protected:
    std::unique_ptr<UI> _child;
    std::vector<std::unique_ptr<UI>> _components;
    std::vector<std::unique_ptr<Element>> _elements;

    template <class T, typename... Args>
    inline T* addComponent(Args&&... args) {
      return static_cast<T*>(_components.emplace_back(std::make_unique<T>(args...)).get());
    }

    template <class T, typename... Args>
    inline T* addElement(Args&&... args) {
      return static_cast<T*>(_elements.emplace_back(std::make_unique<T>(args...)).get());
    }

    virtual bool touch(uint8_t count, GTPoint* points);
    virtual bool release(uint8_t count, GTPoint* points);
  public:
    struct Encoder {
      enum class Rotation : uint8_t {
        CW,
        CCW
      };
      enum class ButtonState : uint8_t {
        IDLE,
        PRESSED,
        RELEASED
      };
      enum class ButtonPress : uint8_t {
        SHORT,
        LONG
      };
    };

    enum class Swipe : uint8_t {
      NONE,
      UP,
      DOWN,
      LEFT,
      RIGHT
    };

    UI();
    virtual ~UI() = default;

    virtual void loop();
    virtual void redraw();

    bool handleTouch(uint8_t count, GTPoint* points, std::function<bool()> touched);
    virtual void encoderRotate(Encoder::Rotation rotation);
    virtual void encoderPress(Encoder::ButtonPress press);
    virtual void swipe(Swipe swipe);

    void reset(bool redrawAfter = false);
};

#endif
