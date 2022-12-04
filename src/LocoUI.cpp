#include <LocoUI.h>
#include <SD.h>
#include <StreamUtils.h>
#include <Settings.h>
#include <Elements/Header.h>
#include <Elements/Button.h>
#include <Functions.h>

LocoUI::LocoUI(DCCExCS& dccExCS, uint16_t address) : _dccExCS(dccExCS), _loco(address) {
  _elements.reserve(37);
  _broadcastLocoHandler = _dccExCS.addEventListener(DCCExCS::Event::BROADCAST_LOCO, std::bind(&LocoUI::broadcast, this, std::placeholders::_1));

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
      functionFile = SD.open("/default.json");
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

  _dccExCS.acquireLoco(address);
}

LocoUI::~LocoUI() {
  _dccExCS.removeEventListener(DCCExCS::Event::BROADCAST_LOCO, _broadcastLocoHandler);
}

void LocoUI::broadcast(void* parameter) {
  UI::tasks.push_back([this, broadcast = *static_cast<DCCExCS::Loco*>(parameter)]() {
    if (broadcast.address == _loco.address) { // Broadcast for loco we're currently controlling
      if (broadcast.speed != _loco.speed) {
        _labelSpeed->setLabel(String(broadcast.speed));
        _loco.speed = broadcast.speed;
      }
      if (broadcast.direction != _loco.direction) {
        _labelDirection->setLabel(broadcast.direction ? "Forward" : "Reverse");
        _loco.direction = broadcast.direction;
      }
      if (_loco.functions == UINT32_MAX) { // First load
        _loco.functions = broadcast.functions;
        createFunctionButtons();
      } else if (broadcast.functions != _loco.functions) {
        toggleFunctionButtons(broadcast.functions);
        _loco.functions = broadcast.functions;
      }
    }
  });
}

void LocoUI::createFunctionButtons() {
  UI::tft->fillRect(0, 90, 320, 344, TFT_BLACK); // Clear buttons
  
  uint8_t i = 0;
  uint16_t y = 92;
  for (JsonArrayConst const& row : _locoFunctions) {
    if (divideAndCeil(++i, 7) == _page) {
      uint8_t cols = row.size();
      uint16_t width = (320 - ((cols - 1) * 7)) / cols;

      uint16_t x = 0;
      uint8_t col = 0;
      for (JsonObjectConst const& fn : row) {
        JsonObjectConst idle = fn["btn"]["idle"];
        JsonObjectConst pressed = fn["btn"]["pressed"];
        // Needed for 4 button rows as it divides to a half pixel so the two inner buttons are 1 pixel wider
        uint8_t extra = cols == 4 && (col == 1 || col == 2) ? 1 : 0;
        bool latching = fn["latching"] | true;
        uint8_t func = fn["fn"];

        auto btn = addElement<Button>(x, y, width + extra, 42,
          Button::Appearance {
            idle["label"].as<const char*>(),
            idle["color"].as<uint16_t>(),
            idle["fill"].as<uint16_t>(),
            idle["border"].as<uint16_t>(),
            idle["icon"].as<const char*>()
          },
          Button::Appearance {
            pressed["label"].as<const char*>(), 
            pressed["color"].as<uint16_t>(),
            pressed["fill"].as<uint16_t>(),
            pressed["border"].as<uint16_t>(),
            pressed["icon"].as<const char*>()
          }, 
          latching,
          _loco.functions.test(func) ? Button::State::PRESSED : Button::State::IDLE
        );
        btn->onTouch([this, latching, func](void* parameter) {
          if (latching) {
            _dccExCS.setLocoFn(_loco.address, func, _loco.functions.flip(func).test(func));
          } else {
            _dccExCS.setLocoFn(_loco.address, func, true);
          }
        });
        btn->onRelease([this, latching, func](void* parameter) {
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
    return dynamic_cast<Button*>(element.get()) != nullptr;
  }), _elements.end());
}

void LocoUI::toggleFunctionButtons(std::bitset<32> toggle) {
  uint8_t i = 0;
  auto btnFirst = std::find_if(_elements.begin(), _elements.end(), [](const auto &element) {
    return dynamic_cast<Button*>(element.get()) != nullptr;
  });
  uint8_t btnIndex = std::distance(_elements.begin(), btnFirst);
  uint8_t btnCount = _elements.size();

  for (JsonArrayConst const& row : _locoFunctions) {
    if (divideAndCeil(++i, 7) == _page) {
      for (JsonObjectConst const& fn : row) {
        uint8_t func = fn["fn"];
        if ((fn["latching"] | true) && _loco.functions.test(func) != toggle.test(func) && btnIndex < btnCount) {
          static_cast<Button*>(_elements[btnIndex].get())
            ->setState(toggle.test(func) ? Button::State::PRESSED : Button::State::IDLE);
        }
        btnIndex++;
      }
    }
  }
}

void LocoUI::encoderRotate(Encoder::Rotation rotation) {
  uint8_t step;
  switch (Settings.LocoUI.speedStep) {
    case SettingsClass::LocoUI::SpeedStep::STEP_1: {
      step = 1;
    } break;
    case SettingsClass::LocoUI::SpeedStep::STEP_2: {
      step = 2;
    } break;
    case SettingsClass::LocoUI::SpeedStep::STEP_4: {
      step = 4;
    } break;
  }

  int16_t speed = _loco.speed + (rotation == Encoder::Rotation::CW ? step : -step);
  _dccExCS.setLocoThrottle(_loco.address, max<int16_t>(min<int16_t>(speed, 126), 0), _loco.direction);
}

void LocoUI::encoderPress(Encoder::ButtonPress press) {
  if (press == Encoder::ButtonPress::SHORT) { // Change loco direction
    _dccExCS.setLocoThrottle(_loco.address, _loco.speed, !_loco.direction);
  }
}

void LocoUI::swipe(Swipe swipe) {
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
}
