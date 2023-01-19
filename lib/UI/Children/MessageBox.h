#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <UI.h>
#include <Elements/Button.h>

using Btn = Button;

class MessageBox : public UI {
  public:
    struct Button {
      const String& label;
      Events::EventCallback callback;
    };

    MessageBox(const String& message, uint16_t color, std::vector<Button> buttons);
    MessageBox(const String& message, uint16_t color);
    MessageBox(const String& message, std::vector<Button> buttons);
    MessageBox(const String& message);
};

#endif
