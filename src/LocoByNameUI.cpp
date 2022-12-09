#include <LocoByNameUI.h>
#include <StreamUtils.h>
#include <Functions.h>
#include <Elements/Header.h>

LocoByNameUI::LocoByNameUI(bool groups) {
  _elements.reserve(9);
  
  addElement<Header>(0, 40, 320, 18, "Select Loco");

  if (groups) { // Load by groups
    if (SD.exists("/groups.json")) {
      File groupsFile = SD.open("/groups.json");
      ReadBufferingStream bufferedFile(groupsFile, _doc.capacity());
      deserializeJson(_doc, bufferedFile);
      groupsFile.close();
    }

    _btnsDoc = _doc.as<JsonArray>();
  } else { // Enum locos directory
    _btnsDoc = _doc.to<JsonArray>();
    File locoDir = SD.open("/locos");
    File locoFile;

    while ((locoFile = locoDir.openNextFile())) {
      if (!locoFile.isDirectory()) {
        addLoco(locoFile);
        locoFile.close();
      }
    }
    locoDir.close();
  }

  drawPagingAndButtons();
}

void LocoByNameUI::addLoco(File& locoFile) {
  StaticJsonDocument<16> filterDoc;
  filterDoc["name"] = true;
  StaticJsonDocument<64> locoDoc;

  ReadBufferingStream bufferedFile(locoFile, locoDoc.capacity());
  deserializeJson(locoDoc, bufferedFile, DeserializationOption::Filter(filterDoc));

  uint16_t address = strtoul(locoFile.name(), (char**)NULL, 10);
  String name = "#";
  name += address;
  name += " - ";
  name += locoDoc["name"].as<const char*>();
  JsonObject btn = _btnsDoc.createNestedObject();
  btn["name"] = name;
  btn["loco"] = address;
}

void LocoByNameUI::drawPagingAndButtons() {
  uint8_t count = _btnsDoc.size();

  if (count > 8) { // If there's more than 8 buttons we need paging
    uint8_t pages = divideAndCeil(count, 8);
    auto paging = addComponent<Paging>(pages);
    paging->addEventListener(Paging::Event::CHANGED, [this](void* parameter) {
      destroyButtons();
      drawButtons(*static_cast<uint8_t*>(parameter));
    });
  } else {
    _components.clear();
  }

  drawButtons();
}

void LocoByNameUI::drawButtons(uint8_t page) {
  UI::tft->fillRect(0, 90, 320, 344, TFT_BLACK); // Clear buttons

  uint16_t y = 70;
  uint8_t i = 0;

  for (JsonObjectConst const& btn : _btnsDoc) {
    if (divideAndCeil(++i, 7) == page) {
      addElement<Button>(0, y, 320, 42, btn["name"].as<const char*>())
        ->onRelease([this, btn](void*) {
          UI::tasks.push_back([this, btn] {
            if (btn.containsKey("locos")) {
              loadGroup(btn["locos"].as<JsonArrayConst>());
            } else {
              uint16_t address = btn["loco"].as<uint16_t>();
              dispatchEvent(Event::SELECTED, &address);
            }
          });
        });
      y += 52;
    }
  }
}

void LocoByNameUI::destroyButtons() {
  _elements.erase(std::remove_if(_elements.begin(), _elements.end(), [](const auto &element) {
    return dynamic_cast<Button*>(element.get()) != nullptr;
  }), _elements.end());
}

void LocoByNameUI::loadGroup(JsonArrayConst locos) {
  _btnsDoc.clear();
  for (uint16_t address : locos) {
    char path[32];
    sprintf(path, "/locos/%d.json", address);
    if (SD.exists(path)) {
      File loco = SD.open(path);
      addLoco(loco);
      loco.close();
    }
  }

  UI::tft->fillRect(0, 435, 320, 42, TFT_BLACK); // Clear paging
  _components.clear();
  destroyButtons();
  drawPagingAndButtons();
}
