#ifndef LOCO_BY_NAME_UI
#define LOCO_BY_NAME_UI

#include <UI.h>
#include <Events.h>
#include <Paging.h>
#include <ArduinoJson.h>

class LocoByNameUI : public UI, public Events {
  private:
    StaticJsonDocument<4096> _doc;
    JsonArray _btnsDoc;
    uint8_t _count = 0;
    std::unique_ptr<Paging> _paging;
  public:
    enum class Event : uint8_t {
      SELECTED
    };

    LocoByNameUI(TFT_eSPI *tft, Tasks *tasks, bool groups);

    void addLoco(File loco);
    void drawPagingAndButtons();
    void drawButtons();
    void destroyButtons();
    void loadGroup(JsonArrayConst locos);

    void rotated();
    void touch(uint8_t count, GTPoint* points);
    void release(uint8_t count, GTPoint* points);
    void encoderRotate(Encoder::Rotation rotation);
};

#endif
