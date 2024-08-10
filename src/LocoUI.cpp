#include <LocoUI.h>
#include <SD.h>
#include <StreamUtils.h>
#include <Settings.h>
#include <Components/Paging.h>
#include <Elements/Header.h>
#include <Elements/Button.h>
#include <Functions.h>

LocoUI::LocoUI(DCCExCS& dccExCS, uint16_t address) : _dccExCS(dccExCS), _loco(address) {
  _elements.reserve(37);
  _broadcastLocoHandler = _dccExCS.addEventListener(DCCExCS::Event::BROADCAST_LOCO, [this](void* parameter) {
    broadcast(parameter);
  });

  char path[32];
  sprintf(path, "/locos/%d.json", _loco.address);
  
  if (SD.exists(path)) { // Check for loco config file
    File locoFile = SD.open(path);
    ReadBufferingStream bufferedFile(locoFile, _locoDoc.capacity());
    deserializeJson(_locoDoc, bufferedFile);
    locoFile.close();
  }

  // Max 20 chars for name
  String loco("#");
  loco += _loco.address;
  if (_locoDoc.containsKey("name")) {
    loco += " - ";
    loco += _locoDoc["name"].as<const char*>();
  }
  addElement<Header>(0, 40, 320, 18, loco);

  addElement<Label>(0, 66, 60, 18, "Speed:");
  _labelSpeed = addElement<Label>(60, 66, 50, 18);

  addElement<Label>(160, 66, 80, 18, "Direction:");
  _labelDirection = addElement<Label>(244, 66, 76, 18);

  if (_locoDoc["functions"].is<JsonArray>()) { // Function map array part of loco json
    _locoFunctions = _locoDoc["functions"].as<JsonArray>();
  } else {
    File functionFile;
    if (!_locoDoc.containsKey("functions") || !(functionFile = SD.open(_locoDoc["functions"].as<const char*>()))) { // Name of function map file
      functionFile = SPIFFS.open("/default.json");
    }

    StaticJsonDocument<16> filter;
    filter["functions"] = true;

    ReadBufferingStream bufferedFile(functionFile, _locoDoc.capacity());
    deserializeJson(_locoDoc, functionFile, DeserializationOption::Filter(filter));
    _locoFunctions = _locoDoc["functions"].as<JsonArray>();
    functionFile.close();
  }

  uint8_t rows = _locoFunctions.size();

  if (rows > 8) { // More than 8 rows and we need paging
    uint8_t pages = divideAndCeil(rows, 7);
    auto paging = addComponent<Paging>(pages);
    paging->addEventListener(Paging::Event::CHANGED, [this](void* parameter) {
      _page = *static_cast<uint8_t*>(parameter);
      destroyFunctionButtons();
      createFunctionButtons();
    });
  }

  createFunctionButtons();

  _dccExCS.acquireLoco(address);
}

LocoUI::~LocoUI() {
  _dccExCS.removeEventListener(DCCExCS::Event::BROADCAST_LOCO, _broadcastLocoHandler);
}

void LocoUI::broadcast(void* parameter) {
  auto broadcast = *static_cast<DCCExCS::Loco*>(parameter);
  if (_loco.address == broadcast.address) { // Broadcast for loco we're currently controlling
    _tasks.push_back([this, loco = _loco, broadcast] {
      if (loco.speed != broadcast.speed) {
        _labelSpeed->setLabel(String(broadcast.speed));
      }
      if (loco.direction != broadcast.direction) {
        _labelDirection->setLabel(broadcast.direction ? "Forward" : "Reverse");
      }
      if (loco.functions != broadcast.functions) {
        toggleFunctionButtons(loco.functions ^ broadcast.functions);
      }
    });
    _loco = broadcast;
    _speedPending = false;
  }
}

void LocoUI::createFunctionButtons() {
  UI::tft->fillRect(0, 90, 320, 344, TFT_BLACK); // Clear buttons
  
  uint8_t i = 0;
  uint16_t y = 92;
  bool paging = _locoFunctions.size() > 8;

  for (JsonArrayConst const& row : _locoFunctions) {
    if (!paging || divideAndCeil(++i, 7) == _page) {
      uint8_t cols = row.size();
      uint16_t width = (320 - ((cols - 1) * 7)) / cols;

      uint16_t x = 0;
      uint8_t col = 0;
      for (JsonObjectConst const& fn : row) {
        // Needed for 4 button rows as it divides to a half pixel so the two inner buttons are 1 pixel wider
        uint8_t extra = cols == 4 && (col == 1 || col == 2) ? 1 : 0;
        bool latching = fn["latching"] | true;
        uint8_t func = fn["fn"];

        auto appearance = [](JsonObjectConst obj) -> Button::Appearance {
          const char* icon = obj["icon"].as<const char*>();
          if (strncmp(icon, "/$/", 3) == 0) {
            return {
              obj["label"].as<const char*>(),
              obj["color"].as<uint16_t>(),
              obj["fill"].as<uint16_t>(),
              obj["border"].as<uint16_t>(),
              icon + 2,
              SPIFFS
            };
          }

          return {
            obj["label"].as<const char*>(),
            obj["color"].as<uint16_t>(),
            obj["fill"].as<uint16_t>(),
            obj["border"].as<uint16_t>(),
            icon
          };
        };

        auto btn = addElement<Button>(x, y, width + extra, 42,
          appearance(fn["btn"]["idle"]),
          appearance(fn["btn"]["pressed"]),
          latching,
          _loco.functions.test(func) ? Button::State::PRESSED : Button::State::IDLE
        );
        btn->onTouch([this, latching, func](void*) {
          if (latching) {
            _dccExCS.setLocoFn(_loco.address, func, _loco.functions.flip(func).test(func));
          } else {
            _dccExCS.setLocoFn(_loco.address, func, true);
          }
        });
        btn->onRelease([this, latching, func](void*) {
          if (!latching) {
            _dccExCS.setLocoFn(_loco.address, func, false);
          }
        });

        x += width + 7 + extra;
        col++;
      }
      y += 49;
    }
  }
}

void LocoUI::destroyFunctionButtons() {
  _elements.erase(std::remove_if(_elements.begin(), _elements.end(), [](const auto &element) {
    return element->getType() == Element::Type::Button;
  }), _elements.end());
}

void LocoUI::toggleFunctionButtons(std::bitset<32> toggle) {
  uint8_t i = 0;
  auto btnFirst = std::find_if(_elements.begin(), _elements.end(), [](const auto &element) {
    return element->getType() == Element::Type::Button;
  });
  uint8_t btnIndex = std::distance(_elements.begin(), btnFirst);
  uint8_t btnCount = _elements.size();
  bool paging = _locoFunctions.size() > 8;

  for (JsonArrayConst const& row : _locoFunctions) {
    if (!paging || divideAndCeil(++i, 7) == _page) {
      for (JsonObjectConst const& fn : row) {
        uint8_t func = fn["fn"];
        if ((fn["latching"] | true) && toggle.test(func) && btnIndex < btnCount) {
          static_cast<Button*>(_elements[btnIndex].get())
            ->setState(_loco.functions.test(func) ? Button::State::PRESSED : Button::State::IDLE);
        }
        btnIndex++;
      }
    }
  }
}

bool LocoUI::encoderRotate(Encoder::Rotation rotation) {
  if (_speedPending) {
    return true;
  }

  int8_t step = rotation == Encoder::Rotation::CW ? 1 : -1;
  switch (Settings.LocoUI.speedStep) {
    case SettingsClass::LocoUI::SpeedStep::STEP_2: {
      step *= 2;
    } break;
    case SettingsClass::LocoUI::SpeedStep::STEP_4: {
      step *= 4;
    } break;
  }
  
  int8_t speed = max<int16_t>(min<int16_t>(_loco.speed + step, 126), 0);
  if (speed != _loco.speed) {
    _speedPending = true;
    _dccExCS.setLocoThrottle(_loco.address, speed, _loco.direction);
  }
  
  return true;
}

bool LocoUI::encoderPress(Encoder::ButtonPress press) {
  if (press == Encoder::ButtonPress::SHORT) { // Change loco direction
    _dccExCS.setLocoThrottle(_loco.address, _loco.speed, !_loco.direction);
  }

  return true;
}

bool LocoUI::swipe(Swipe swipe) {
  switch (swipe) {
    case Swipe::UP: {
      dispatchEvent(Event::SWIPE_ACTION, &Settings.LocoUI.Swipe.up);
    } break;
    case Swipe::DOWN: {
      dispatchEvent(Event::SWIPE_ACTION, &Settings.LocoUI.Swipe.down);
    } break;
    case Swipe::LEFT: {
      dispatchEvent(Event::SWIPE_ACTION, &Settings.LocoUI.Swipe.left);
    } break;
    case Swipe::RIGHT: {
      dispatchEvent(Event::SWIPE_ACTION, &Settings.LocoUI.Swipe.right);
    } break;
  }

  return true;
}
