#ifndef MENU_UI_H
#define MENU_UI_H

#include <UI.h>
#include <Events.h>
#include <DCCExCS.h>

using Btn = Button;

class MenuUI : public UI, public Events {
  public:
    enum class Event : uint8_t {
      SELECTED
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
      SERVER,
      SETTINGS
    };
  private:
    DCCExCS::Power *power;

    Btn *powerAll;
    Btn *powerMain;
    Btn *powerProg;
    Btn *powerJoin;

    void selected(Button button);
  public:
    MenuUI(TFT_eSPI *tft, Tasks *tasks, DCCExCS::Power *power);

    void csPowerChange();
};

#endif
