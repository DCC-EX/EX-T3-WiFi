#include <MessageBox.h>
#include <Elements/Label.h>

MessageBox::MessageBox(const String& message, uint16_t color, std::vector<Button> buttons) {
  addElement<Label>(0, 200, 320, 40, message, true, color);

  uint16_t widths[] = {
    320,
    157,
    102
  };
  uint16_t w = widths[buttons.size() - 1];
  uint16_t x = 0;

  for (const auto& button : buttons) {
    addElement<Btn>(x, 250, w, 40, button.label)
      ->onRelease(std::move(button.callback));
    x += w + 6;
  }
}

MessageBox::MessageBox(const String& message, uint16_t color)
    : MessageBox(message, color, { }) { }

MessageBox::MessageBox(const String& message, std::vector<Button> buttons)
    : MessageBox(message, TFT_WHITE, buttons) { }

MessageBox::MessageBox(const String& message)
    : MessageBox(message, TFT_WHITE, { }) { }
