#ifndef MENU_UI_H
#define MENU_UI_H

#include <UI.h>
#include <Events.h>
#include <Elements/Button.h>
#include <DCCExCS.h>

using Btn = Button;

class MenuUI : public UI, public Events {
  public:
    struct Event {
      static constexpr uint8_t SELECTED = 0;
    };

    enum class Button : uint8_t {
      LOCO_LOAD_BY_ADDRESS,
      LOCO_LOAD_BY_NAME,
      LOCO_LOAD_BY_GROUP,
      LOCO_RELEASE,
      LOCO_PROGRAM,
      ACCESSORY_ON,
      ACCESSORY_OFF,
      POWER_OFF_ALL,
      POWER_ON_ALL,
      POWER_OFF_MAIN,
      POWER_ON_MAIN,
      POWER_OFF_PROG,
      POWER_ON_PROG,
      POWER_JOIN,
      WIFI,
      SETTINGS,
      ABOUT
    };
  private:
    DCCExCS& _dccExCS;
    DCCExCS::Power& _power;
    uint8_t _broadcastPowerHandler;

    Btn* _powerAll;
    Btn* _powerMain;
    Btn* _powerProg;
    Btn* _powerJoin;

    void selected(Button button);
  public:
    MenuUI(DCCExCS& dccExCS, DCCExCS::Power& power);
    ~MenuUI();
};

#endif
