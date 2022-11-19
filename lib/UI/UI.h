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
    using Tasks = std::deque<std::function<void (void)>>;
  protected:
    TFT_eSPI *_tft;
    Tasks *_tasks;
    std::vector<std::unique_ptr<Element>> _elements;

    template <class T, typename... Args>
    inline T* addElement(Args&&... args) {
      return static_cast<T*>(_elements.emplace_back(std::make_unique<T>(_tft, std::forward<Args>(args)...)).get());
    }
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

    UI(TFT_eSPI *tft, Tasks *tasks);
    virtual ~UI() = default;

    virtual void loop();
    virtual void rotated();

    virtual void touch(uint8_t count, GTPoint* points);
    virtual void release(uint8_t count, GTPoint* points);
    virtual void encoderRotate(Encoder::Rotation rotation);
    virtual void encoderPress(Encoder::ButtonPress press);
    virtual void swipe(Swipe swipe);
};

#endif
