#include <MessageBox.h>
#include <Elements/Label.h>

MessageBox::MessageBox(const String& message, uint16_t color, std::vector<Button> buttons) {
  uint8_t lines = 0;
  int16_t lastLF = -1;
  while ((lastLF = message.indexOf("\n", lastLF + 1)) != -1) {
    lines++;
  }

  _elements.reserve(lines + 1 + buttons.size());

  lastLF = -1;
  uint16_t y = 252 - (lines * 20) - (buttons.size() ? 40 : 0);
  uint8_t sub = 0;
  for (uint8_t i = 0; i <= lines; i++) {
    lastLF = message.indexOf("\n", lastLF + 1);
    addElement<Label>(0, y, 320, 18, message.substring(sub, lastLF == -1 ? message.length() : lastLF), true, color);
    sub = lastLF + 1;
    y += 20;
  }

  if (buttons.size()) {
    uint16_t widths[] = { 320, 157, 102 };
    uint16_t w = widths[buttons.size() - 1];
    uint16_t x = 0;

    for (const auto& button : buttons) {
      addElement<Btn>(x, y + 6, w, 40, button.label)
        ->onRelease(std::move(button.callback));
      x += w + 6;
    }
  }
}

MessageBox::MessageBox(const String& message, std::vector<Button> buttons)
    : MessageBox(message, TFT_WHITE, buttons) { }

MessageBox::MessageBox(const String& message, uint16_t color)
    : MessageBox(message, color, { }) { }

MessageBox::MessageBox(const String& message)
    : MessageBox(message, TFT_WHITE, { }) { }
