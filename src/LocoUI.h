#ifndef LOCO_UI_H
#define LOCO_UI_H

#include <bitset>
#include <UI.h>
#include <DCCExCS.h>
#include <Elements/Label.h>
#include <ArduinoJson.h>

class LocoUI : public UI, public Events {
  public:
    struct Event {
      static constexpr uint8_t SWIPE_ACTION = 0;
    };
  private:
    DCCExCS& _dccExCS;
    DCCExCS::Loco _loco;
    uint8_t _broadcastLocoHandler;
    int8_t _antiFlood = 0;

    Label* _labelSpeed;
    Label* _labelDirection;

    StaticJsonDocument<10240> _locoDoc;
    JsonArrayConst _locoFunctions;
    uint8_t _page = 1;

    void broadcast(void* parameter);
    void createFunctionButtons();
    void destroyFunctionButtons();
    void toggleFunctionButtons(std::bitset<32> toggle);
  public:
    LocoUI(DCCExCS& dccExCS, uint16_t address);
    ~LocoUI();

    bool encoderRotate(Encoder::Rotation rotation);
    bool encoderPress(Encoder::ButtonPress press);
    bool swipe(Swipe swipe);
};

#endif
