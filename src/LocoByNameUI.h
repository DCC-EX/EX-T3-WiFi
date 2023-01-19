#ifndef LOCO_BY_NAME_UI
#define LOCO_BY_NAME_UI

#include <SD.h>
#include <UI.h>
#include <Events.h>
#include <ArduinoJson.h>

class LocoByNameUI : public UI, public Events {
  private:
    StaticJsonDocument<4096> _doc;
    JsonArray _btnsDoc;

    void addLoco(File& loco);
    void drawPagingAndButtons();
    void drawButtons(uint8_t page = 1);
    void destroyButtons();
    void loadGroup(JsonArrayConst locos);
  public:
    struct Event {
      static constexpr uint8_t SELECTED = 0;
    };

    LocoByNameUI(bool groups);
};

#endif
