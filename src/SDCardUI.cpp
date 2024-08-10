#include <SDCardUI.h>
#include <Elements/Label.h>
#include <Elements/Button.h>
#include <Children/MessageBox.h>

SDCardUI::SDCardUI() {
  _elements.reserve(11);

  addElement<Label>(0, 33, 320, 18, "Unable to mount SD Card", true, TFT_RED);
  
  addElement<Label>(0, 87, 320, 18, "No SD card detected ", true);
  addElement<Label>(0, 114, 320, 18, "or it's an invalid format", true);

  addElement<Label>(0, 168, 320, 18, "If an SD card is inserted and you believe", true);
  addElement<Label>(0, 195, 320, 18, "this is an error try turning the throttle", true);
  addElement<Label>(0, 222, 320, 18, "off and on or if this is a new SD card", true);
  addElement<Label>(0, 249, 320, 18, "press the button below to format", true);

  addElement<Label>(0, 303, 320, 18, "If successful the throttle will restart", true, TFT_GREEN);

  addElement<Label>(0, 357, 320, 18, "*WARNING*", true, TFT_RED);
  addElement<Label>(0, 384, 320, 18, "Formatting will erase the SD card", true, TFT_RED);

  addElement<Button>(0, 438, 320, 42, "Format")->onRelease([this](void*) {
    _tasks.push_back([this] {
      setChild<MessageBox>("Formatting...", TFT_BLUE);

      if (SD.begin(D7, SPI, 4000000, "/sd", 5, true)) {
        ESP.restart();
      } else {
        _tasks.push_back([this] {
          setChild<MessageBox>("Failed formatting...", TFT_RED, std::vector<MessageBox::Button> {
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
  });
}
