#include <Pin.h>
#include <MessageBox.h>

Pin::Pin(uint32_t pin) : Keypad("Enter Pin"), _pin(pin) {
  addEventListener(Event::ENTER, [this](void* parameter) {
    if (*static_cast<uint32_t*>(parameter) == _pin) {
      dispatchEvent(Event::VALID);
    } else {
      _tasks.push_back([this] {
        _number->setValue("", false);
        setChild<MessageBox>("Incorrect Pin", TFT_RED, std::vector<MessageBox::Button> {
          {
            "Ok",
            [this](void*) {
              _tasks.push_back([this] {
                reset(true);
              });
            }
          },
        });
      });
    }
  });
}
