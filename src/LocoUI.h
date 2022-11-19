#ifndef LOCO_UI_H
#define LOCO_UI_H

#include <bitset>
#include <UI.h>
#include <DCCExCS.h>
#include <Elements/Label.h>
#include <Paging.h>
#include <ArduinoJson.h>

class LocoUI : public UI, public Events {
  public:
    enum class Event : uint8_t {
      SWIPE_ACTION
    };
  private:
    DCCExCS *_dccExCS;
    DCCExCS::Loco _loco;
    uint8_t _broadcastLocoHandler;

    Label *_labelSpeed;
    Label *_labelDirection;

    StaticJsonDocument<10240> _locoDoc;
    JsonArrayConst _locoFunctions;
    std::unique_ptr<Paging> _paging;

    void broadcast(void *parameter);
    void createFunctionButtons();
    void destroyFunctionButtons();
    void toggleFunctionButtons(std::bitset<32> toggle);
  public:
    LocoUI(TFT_eSPI *tft, Tasks *tasks, DCCExCS *dccExCS, uint16_t address);
    ~LocoUI();

    void rotated();
    void touch(uint8_t count, GTPoint* points);
    void release(uint8_t count, GTPoint* points);
    void encoderRotate(Encoder::Rotation rotation);
    void encoderPress(Encoder::ButtonPress press);
    void swipe(Swipe swipe);
};

#endif
