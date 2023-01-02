#include <MenuUI.h>
#include <Elements/Header.h>
#include <Elements/Button.h>

MenuUI::MenuUI(DCCExCS& dccExCS, DCCExCS::Power& power) : _dccExCS(dccExCS), _power(power) {
  _elements.reserve(17);
  _broadcastPowerHandler = dccExCS.addEventListener(DCCExCS::Event::BROADCAST_POWER, [this](void*) {
    _tasks.push_back([this] {
      _powerAll->setState(_power.main && _power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
      _powerMain->setState(_power.main ? Btn::State::PRESSED : Btn::State::IDLE);
      _powerProg->setState(_power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
      _powerJoin->setState(_power.join ? Btn::State::PRESSED : Btn::State::IDLE);
    });
  });

  addElement<Header>(0, 40, 320, 18, "Loco");

  addElement<Btn>(0, 70, 102, 42, "By Address")
    ->onRelease([this](void*) {
      selected(Button::LOCO_LOAD_BY_ADDRESS);
    });

  addElement<Btn>(109, 70, 102, 42, "By Name")
    ->onRelease([this](void*) {
      selected(Button::LOCO_LOAD_BY_NAME);
    });

  addElement<Btn>(218, 70, 102, 42, "By Group")
    ->onRelease([this](void*) {
      selected(Button::LOCO_LOAD_BY_GROUP);
    });

  addElement<Btn>(0, 119, 157, 42, "Release")
    ->onRelease([this](void*) {
      selected(Button::LOCO_RELEASE);
    });

  addElement<Btn>(163, 119, 157, 42, "Program")
    ->onRelease([this](void*) {
      selected(Button::LOCO_PROGRAM);
    });

  addElement<Header>(0, 173, 320, 18, "Accessories");

  addElement<Btn>(0, 203, 157, 42, "On")
    ->onRelease([this](void*) {
      selected(Button::ACCESSORY_ON);
    });

  addElement<Btn>(163, 203, 157, 42, "Off")
    ->onRelease([this](void*) {
      selected(Button::ACCESSORY_OFF);
    });

  addElement<Header>(0, 257, 320, 18, "Power");

  _powerAll = addElement<Btn>(0, 287, 102, 42, "On All", "Off All", true, power.main && power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  _powerAll->onRelease([this](void*) {
    selected(_powerAll->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_ALL
      : Button::POWER_ON_ALL);
  });

  _powerMain = addElement<Btn>(109, 287, 102, 42, "On Main", "Off Main", true, power.main ? Btn::State::PRESSED : Btn::State::IDLE);
  _powerMain->onRelease([this](void*) {
    selected(_powerMain->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_MAIN
      : Button::POWER_ON_MAIN);
  });

  _powerProg = addElement<Btn>(218, 287, 102, 42, "On Prog", "Off Prog", true, power.prog ? Btn::State::PRESSED : Btn::State::IDLE);
  _powerProg->onRelease([this](void*) {
    selected(_powerProg->getState() == Btn::State::IDLE
      ? Button::POWER_OFF_PROG
      : Button::POWER_ON_PROG);
  });

  _powerJoin = addElement<Btn>(0, 336, 320, 42, "Join Tracks", true, power.join ? Btn::State::PRESSED : Btn::State::IDLE);
  _powerJoin->onRelease([this](void*) {
    selected(Button::POWER_JOIN);
  });

  addElement<Header>(0, 390, 320, 18, "Settings");

  addElement<Btn>(0, 420, 157, 42, "T3 WiFi")
    ->onRelease([this](void*) {
      selected(Button::WIFI);
    });

  addElement<Btn>(163, 420, 157, 42, "Settings")
    ->onRelease([this](void*) {
      selected(Button::SETTINGS);
    });
}

MenuUI::~MenuUI() {
  _dccExCS.removeEventListener(DCCExCS::Event::BROADCAST_POWER, _broadcastPowerHandler);
}

void MenuUI::selected(MenuUI::Button button) {
  dispatchEvent(Event::SELECTED, &button);
}
