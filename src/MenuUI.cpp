#include <MenuUI.h>
#include <Elements/Header.h>
#include <Elements/Button.h>

MenuUI::MenuUI(DCCExCS::Power& power) : power(power) {
  _elements.reserve(17);

  addElement<Header>(0, 40, 320, 18, "Loco");

  addElement<Btn>(0, 70, 102, 42, "By Address")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::LOCO_LOAD_BY_ADDRESS));

  addElement<Btn>(109, 70, 102, 42, "By Name")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::LOCO_LOAD_BY_NAME));

  addElement<Btn>(218, 70, 102, 42, "By Group")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::LOCO_LOAD_BY_GROUP));

  addElement<Btn>(0, 119, 157, 42, "Release")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::LOCO_RELEASE));

  addElement<Btn>(163, 119, 157, 42, "Program")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::LOCO_PROGRAM));

  addElement<Header>(0, 173, 320, 18, "Accessories");

  addElement<Btn>(0, 203, 157, 42, "On")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::ACCESSORY_ON));

  addElement<Btn>(163, 203, 157, 42, "Off")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::ACCESSORY_OFF));

  addElement<Header>(0, 257, 320, 18, "Power");

  powerAll = addElement<Btn>(0, 287, 102, 42, "On All", "Off All", true, power.main && power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  powerAll->onRelease([this](void* parameter) {
    selected(static_cast<Btn*>(parameter)->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_ALL
      : Button::POWER_ON_ALL);
  });

  powerMain = addElement<Btn>(109, 287, 102, 42, "On Main", "Off Main", true, power.main ? Btn::State::PRESSED : Btn::State::IDLE);
  powerMain->onRelease([this](void* parameter) {
    selected(static_cast<Btn*>(parameter)->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_MAIN
      : Button::POWER_ON_MAIN);
  });

  powerProg = addElement<Btn>(218, 287, 102, 42, "On Prog", "Off Prog", true, power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  powerProg->onRelease([this](void* parameter) {
    selected(static_cast<Btn*>(parameter)->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_PROG
      : Button::POWER_ON_PROG);
  });

  powerJoin = addElement<Btn>(0, 336, 320, 42, "Join Tracks", true, power.join ? Btn::State::PRESSED : Btn::State::IDLE);
  powerJoin->onRelease(std::bind(&MenuUI::selected, this, Button::POWER_JOIN));

  addElement<Header>(0, 390, 320, 18, "Settings");

  addElement<Btn>(0, 420, 157, 42, "T3 Web Server")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::SERVER));

  addElement<Btn>(163, 420, 157, 42, "Settings")
    ->onRelease(std::bind(&MenuUI::selected, this, Button::SETTINGS));
}

void MenuUI::selected(MenuUI::Button button) {
  dispatchEvent(Event::SELECTED, &button);
}

void MenuUI::csPowerChange() {
  powerAll->setState(power.main && power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  powerMain->setState(power.main ? Btn::State::PRESSED : Btn::State::IDLE);
  powerProg->setState(power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  powerJoin->setState(power.join ? Btn::State::PRESSED : Btn::State::IDLE);
}
