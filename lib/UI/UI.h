#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <GT911.h>
#include <Component.h>

#include <vector>
#include <deque>

class UI : public Component {
  public:
    static inline TFT_eSPI* tft;
  protected:
    std::deque<std::function<void (void)>> _tasks;
    std::unique_ptr<UI> _child;
    std::vector<std::unique_ptr<Component>> _components;

    template <class T, typename... Args>
    inline T* setChild(Args&&... args) {
      reset();
      _child = std::make_unique<T>(args...);
      return static_cast<T*>(_child.get());
    }

    template <class T, typename... Args>
    inline T* addComponent(Args&&... args) {
      return static_cast<T*>(_components.emplace_back(std::make_unique<T>(args...)).get());
    }

    virtual void loop(); 
  public:
    virtual ~UI() = default;

    void handleRedraw();
    bool handleTouch(uint8_t count, GTPoint* points, std::function<bool()> touched);
    void handleEncoderRotate(Encoder::Rotation rotation);
    void handleEncoderPress(Encoder::ButtonPress press);
    void handleSwipe(Swipe swipe);
    void handleTasks();
    void handleLoop();

    void reset(bool redraw = false);
};

#endif
