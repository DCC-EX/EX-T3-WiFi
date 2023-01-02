#ifndef COMPONENT_H
#define COMPONENT_H

#include <Arduino.h>
#include <GT911.h>
#include <Elements/Element.h>

#include <vector>

class Component {
  friend class UI;
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
  protected:
    std::vector<std::unique_ptr<Element>> _elements;

    template <class T, typename... Args>
    inline T* addElement(Args&&... args) {
      return static_cast<T*>(_elements.emplace_back(std::make_unique<T>(args...)).get());
    }

    virtual void redraw();
    virtual bool touch(uint8_t count, GTPoint* points);
    virtual bool release(uint8_t count, GTPoint* points);
    virtual bool encoderRotate(Encoder::Rotation rotation);
    virtual bool encoderPress(Encoder::ButtonPress press);
    virtual bool swipe(Swipe swipe);
};

#endif
