#include <Arduino.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <GT911.h>
#include <DFRobot_LIS2DW12.h>
#undef ERR_OK // Needed as the DFRobot_LIS2DW12.h header has an unused define that conflicts
#include <Encoder.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <Settings.h>
#include <DCCExCS.h>
#include <Locos.h>
#include <UI.h>
#include <UIHeader.h>
#include <Keypad.h>
#include <MenuUI.h>
#include <WiFiUI.h>
#include <ProgramUI.h>
#include <LocoUI.h>
#include <LocoByNameUI.h>
#include <SettingsUI.h>

const uint32_t POWER_CHECK = 60000 * 2; // 2 Minutes
const uint8_t BATTERY_PIN = A2;
const uint8_t GESTURE_DISTANCE = 15;
const uint16_t CONNECTION_ALIVE_DELAY = 2500;
const uint16_t CONNECTION_TIMEOUT = 10000;

const uint8_t ENCODER_BTN = A3;
const uint8_t ENCODER_B = A1;
const uint8_t ENCODER_A = A0;

TFT_eSPI tft;
GT911 ts;
#if defined(USE_ACCELEROMETER)
DFRobot_LIS2DW12_I2C acce;
DFRobot_LIS2DW12::eOrient_t lastOrientation = DFRobot_LIS2DW12::eOrient_t::eYDown;
#endif

Encoder* encoder;
uint32_t encoderPressMillis = 0;
UI::Encoder::ButtonState encoderBtnState = UI::Encoder::ButtonState::IDLE;
uint8_t currentEncoderPinState, lastEncoderPinState;

AsyncClient csClient;
uint8_t csBuffer[1024];
uint16_t csBufferLen = 0;
DCCExCS dccExCS(csClient);
DCCExCS::Power power;
Locos locos;

std::unique_ptr<UIHeader> uiHeader;
std::unique_ptr<UI> activeUI;

void setMenuUI();
void setLocoUI();

template <typename T>
void setUI(T&& ui) {
  UI::tft->fillRect(0, 30, 320, 450, TFT_BLACK);
  UI::tasks.push_back([ui]() {
    activeUI.reset();
    activeUI = ui();
  });
}

void setLocoKeypadUI(const Events::EventCallback &&onCancel) {
  setUI([onCancel]() {
    auto ui = std::make_unique<Keypad>("Loco Address", 10293, 0);
    ui->addEventListener(Keypad::Event::ENTER, [](void* parameter) {
      locos.add(*static_cast<uint16_t*>(parameter));
      setLocoUI();
    });
    ui->addEventListener(Keypad::Event::CANCEL, std::move(onCancel));
    return ui;
  });
}

void setLocoByNameUI(bool group) {
  setUI([group]() {
    auto ui = std::make_unique<LocoByNameUI>(group);
    ui->addEventListener(LocoByNameUI::Event::SELECTED, [](void* parameter) {
      locos.add(*static_cast<uint16_t*>(parameter));
      setLocoUI();
    });
    return ui;
  });
}

void setLocoUI() {
  setUI([]() {
    auto ui = std::make_unique<LocoUI>(dccExCS, locos);
    ui->addEventListener(LocoUI::Event::SWIPE_ACTION, [](void* parameter) {
      switch (const auto action = *static_cast<uint8_t*>(parameter)) {
        case SettingsClass::LocoUI::Swipe::Action::KEYPAD: {
          setLocoKeypadUI([](void* parameter) {
            if (locos == 0) {
              setMenuUI();
            } else {
              setLocoUI();
            }
          });
        } break;
        case SettingsClass::LocoUI::Swipe::Action::NAME:
        case SettingsClass::LocoUI::Swipe::Action::GROUP: {
          bool group = action == SettingsClass::LocoUI::Swipe::Action::GROUP;
          setLocoByNameUI(group);
        } break;
        case SettingsClass::LocoUI::Swipe::Action::RELEASE: {
          dccExCS.releaseLoco(locos.remove());
          // if the action after swipe release is prev/next and there's no active locos then go back to menu
          if ((Settings.LocoUI.Swipe.release == SettingsClass::LocoUI::Swipe::Action::NEXT 
                || Settings.LocoUI.Swipe.release == SettingsClass::LocoUI::Swipe::Action::PREV
              ) && locos == 0) {
            setMenuUI();
          } else {
            (static_cast<LocoUI*>(activeUI.get()))->dispatchEvent(LocoUI::Event::SWIPE_ACTION, &Settings.LocoUI.Swipe.release);
          }
        } break;
        case SettingsClass::LocoUI::Swipe::Action::NEXT: {
          locos.next();
          setLocoUI();
        } break;
        case SettingsClass::LocoUI::Swipe::Action::PREV: {
          locos.prev();
          setLocoUI();
        } break;
      }
    });

    return ui;
  });
}

void setAccessoryKeypadUI(bool state) {
  setUI([state]() {
    auto ui = std::make_unique<Keypad>("Accessory Address", 2044, 1);
    ui->addEventListener(Keypad::Event::ENTER, [state](void* parameter) {
      dccExCS.accessory(*static_cast<uint16_t*>(parameter), state);
      setMenuUI();
    });
    ui->addEventListener(Keypad::Event::CANCEL, [](void* parameter) {
      setMenuUI();
    });
    return ui;
  });
}

void setMenuUI() {
  setUI([]() {
    auto ui = std::make_unique<MenuUI>(dccExCS, power);
    ui->addEventListener(MenuUI::Event::SELECTED, [](void* parameter) {
      switch (const auto button = *static_cast<MenuUI::Button*>(parameter)) {
        case MenuUI::Button::LOCO_LOAD_BY_ADDRESS: {
          setLocoKeypadUI([](void* parameter) {
            setMenuUI();
          });
        } break;
        case MenuUI::Button::LOCO_LOAD_BY_NAME:
        case MenuUI::Button::LOCO_LOAD_BY_GROUP: {
          bool group = button == MenuUI::Button::LOCO_LOAD_BY_GROUP;
          setLocoByNameUI(group);
        } break;
        case MenuUI::Button::LOCO_RELEASE: {
          dccExCS.releaseLoco(locos.remove());
        } break;
        case MenuUI::Button::LOCO_PROGRAM: {
          setUI([]() {
            return std::make_unique<ProgramUI>(dccExCS);
          });
        } break;
        case MenuUI::Button::ACCESSORY_ON:
        case MenuUI::Button::ACCESSORY_OFF: {
          bool state = button == MenuUI::Button::ACCESSORY_ON;
          setAccessoryKeypadUI(state);
        } break;
        case MenuUI::Button::POWER_ON_ALL:
        case MenuUI::Button::POWER_OFF_ALL: {
          bool state = button == MenuUI::Button::POWER_ON_ALL;
          dccExCS.setCSPower(DCCExCS::Power::ALL, state);
        } break;
        case MenuUI::Button::POWER_ON_MAIN:
        case MenuUI::Button::POWER_OFF_MAIN: {
          bool state = button == MenuUI::Button::POWER_ON_MAIN;
          dccExCS.setCSPower(DCCExCS::Power::MAIN, state);
        } break;
        case MenuUI::Button::POWER_ON_PROG:
        case MenuUI::Button::POWER_OFF_PROG: {
          bool state = button == MenuUI::Button::POWER_ON_PROG;
          dccExCS.setCSPower(DCCExCS::Power::PROG, state);
        } break;
        case MenuUI::Button::POWER_JOIN: {
          dccExCS.setCSPower(DCCExCS::Power::JOIN, true);
        } break;
        case MenuUI::Button::WIFI: {
          setUI([]() {
            return std::make_unique<WiFiUI>();
          });
        } break;
        case MenuUI::Button::SETTINGS: {
          setUI([]() {
            return std::make_unique<SettingsUI>();
          });
        } break;
      }
    });
    return ui;
  });
}

// Based off the helpful blog post here, https://savjee.be/2020/02/esp32-keep-wifi-alive-with-freertos-task/
void keepConnectionsAlive(void* parameter) {
  for (;;) {
    if (Settings.CS.valid()) { // Valid if we have SSID, Server & Port
      if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(Settings.CS.SSID().c_str(), Settings.CS.password().c_str());

        uint32_t timeout = millis() + CONNECTION_TIMEOUT;
        while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
          vTaskDelay(250 / portTICK_PERIOD_MS);
        }
      }
      
      if (WiFi.status() == WL_CONNECTED && !csClient.connected()) {
        csClient.connect(Settings.CS.server().c_str(), Settings.CS.port()); 
      }
      vTaskDelay(CONNECTION_ALIVE_DELAY / portTICK_PERIOD_MS);
    }
  }
}

void powerCheck(void* parameter) {
  for (;;) {
    uint32_t total = 0;
    for (uint8_t i = 0; i < 10; i++) {
      total += analogReadMilliVolts(BATTERY_PIN);
      delay(100);
    }

    float voltage = total / 10;
    voltage *= 2;
    voltage /= 1000;

    UI::tasks.push_back([voltage]() {
      uiHeader->setPowerStatus(voltage);
    });
    vTaskDelay(POWER_CHECK / portTICK_PERIOD_MS);
  }
}

void setRotation() {
  bool standard = Settings.rotation == SettingsClass::Rotation::STANDARD;
  #if defined(USE_ACCELEROMETER)
  if (Settings.rotation == SettingsClass::Rotation::ACCELEROMETER) {
    standard = lastOrientation == DFRobot_LIS2DW12::eOrient_t::eYDown;
  }
  #endif

  UI::tft->setRotation(standard ? 2 : 0);
  ts.setRotation(standard ? GT911::Rotate::_180 : GT911::Rotate::_0);

  if (activeUI != nullptr) {
    UI::tasks.push_back([]() {
      UI::tft->fillScreen(TFT_BLACK);
      uiHeader->redraw();
      activeUI->redraw();
    });
  }
}

void setup() {
  Serial.begin(115200);

  // Start file systems
  SPIFFS.begin();
  uint8_t tries = 3;
  while (!SD.begin(D7) && tries-- > 0) {
    delay(100);
  }
  // TODO, if we fail to start file system then restart?

  // Load the settings
  Settings.load();
  Settings.addEventListener(SettingsClass::Event::ROTATION_CHANGE, [](void* parameter) {
    #if defined(USE_ACCELEROMETER)
    lastOrientation = acce.getOrientation();
    #endif
    setRotation();    
  });

  // Start the TFT display
  UI::tft = &tft;
  UI::tft->init();
  UI::tft->fillScreen(TFT_BLACK);
  UI::tft->setTextColor(TFT_WHITE, TFT_BLACK);
  UI::tft->loadFont("fonts/FreeSans-18");

  // pinMode(12, OUTPUT);
  // digitalWrite(12, HIGH); // Keep backlight on
  // digitalWrite(12, LOW); // Turn off backlight
  // TODO, screen standby? can it remember buffer

  // Start the touchscreen
  if (ts.begin()) {
    // Tweak the touchscreen borders
    GTConfig* cfg = ts.readConfig();
    cfg->hSpace = (5 | (5 << 4));
    cfg->vSpace = (5 | (5 << 4));
    ts.writeConfig();
  }

  #if defined(USE_ACCELEROMETER)
  // Start the accelerometer
  acce.begin();
  acce.softReset();
  acce.setRange(DFRobot_LIS2DW12::e2_g);
  acce.setPowerMode(DFRobot_LIS2DW12::eContLowPwrLowNoise1_12bit);
  acce.setDataRate(DFRobot_LIS2DW12::eRate_200hz);
  acce.set6DThreshold(DFRobot_LIS2DW12::eDegrees60);
  acce.setInt1Event(DFRobot_LIS2DW12::e6D);
  delay(25);
  lastOrientation = acce.getOrientation();
  #endif

  // Setup encoder
  pinMode(ENCODER_BTN, INPUT);
  // TODO, fork encoder and break out with begin method? need to construct here as the class constructor tries to set pins before the ESP32 is ready
  encoder = new Encoder(ENCODER_B, ENCODER_A);

  // Setup the WiFi
  WiFi.setHostname(THROTTLE_AP_NAME);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  
  // WiFi connected
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    UI::tasks.push_back([]() {
      uiHeader->setWiFiStatus(true);
    });
  }, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  // WiFi disconnected
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    UI::tasks.push_back([]() {
      uiHeader->setWiFiStatus(false);
    });
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  // CS connected
  csClient.onConnect([](void* arg, AsyncClient* client) {
    UI::tasks.push_back([]() {
      dccExCS.getCSPower(); // Get current power status
      uiHeader->setCSStatus(true);
    });
  });
  // CS disconnected
  csClient.onDisconnect([](void* arg, AsyncClient* client) {
    UI::tasks.push_back([]() {
      uiHeader->setCSStatus(false);
    });
  });
  // If the connection to the CS times out then close it so it'll attempt a reconnect
  csClient.onTimeout([](void* arg, AsyncClient* client, uint32_t time) {
    UI::tasks.push_back([]() {
      csClient.close(true);
    });
  });
  // CS data
  csClient.onData([](void* arg, AsyncClient* client, void* data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
      if (static_cast<uint8_t*>(data)[i] == '<') { // Start of cmd
        csBuffer[0] = '\0';
        csBufferLen = 0;
      } else if (static_cast<uint8_t*>(data)[i] == '>') { // End of cmd, pass onto ex cs handler
        csBuffer[csBufferLen] = '\0';
        dccExCS.handleCS(csBuffer, csBufferLen);
      } else {
        // TODO, only add to buffer if we're in a command?
        csBuffer[csBufferLen++] = static_cast<uint8_t*>(data)[i];
      }
    }
  });

  // Aquired loco count change
  locos.addEventListener(Locos::Event::COUNT_CHANGE, [](void* parameter) {
    UI::tasks.push_back([count = *static_cast<uint8_t*>(parameter)]() {
      uiHeader->setLocoCount(count);
    });
  });

  // CS Power event
  dccExCS.addEventListener(DCCExCS::Event::BROADCAST_POWER, [](void* parameter) {
    power = *static_cast<DCCExCS::Power*>(parameter); // Remember current power states
  });

  // If the CS settings change we disconnect so the keep alive task will reconnect using new values
  Settings.addEventListener(SettingsClass::Event::CS_CHANGE, [](void* parameter) {
    // Done in task queue otherwise we crash out with watchdog timer
    UI::tasks.push_back([]() {
      csClient.close();
      WiFi.disconnect();
    });
  });

  xTaskCreatePinnedToCore(powerCheck, "powerCheck", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(keepConnectionsAlive, "keepConnectionsAlive", 2048, NULL, 1, NULL, 1);

  // Set initial rotation before any display output
  setRotation();
  // Create UI Header
  uiHeader = std::make_unique<UIHeader>();
  uiHeader->addEventListener(UIHeader::Event::MENU, [](void* parameter) {
    if (dynamic_cast<MenuUI*>(activeUI.get()) != nullptr) { // Is current UI the menu?
      if (locos != 0) { // If we're already on the menu and there's an active loco switch to the loco UI
        setLocoUI();
      }
    } else { // Load the menu UI
      setMenuUI();
    }
  });
  // Start with the menu
  setMenuUI();
}

void loop() {
  // Encoder press
  currentEncoderPinState = digitalRead(ENCODER_BTN);
  if (currentEncoderPinState != lastEncoderPinState) {
    if (millis() - encoderPressMillis > 50) { // Debounce
      if (currentEncoderPinState == LOW && encoderBtnState == UI::Encoder::ButtonState::IDLE) { // Press
        encoderBtnState = UI::Encoder::ButtonState::PRESSED;
      } else if (currentEncoderPinState == HIGH && encoderBtnState == UI::Encoder::ButtonState::PRESSED) { // Release
        encoderBtnState = UI::Encoder::ButtonState::RELEASED;
      }
    }
    encoderPressMillis = millis();
  }
  lastEncoderPinState = currentEncoderPinState;

  uint8_t touches = ts.touched(GT911_MODE_POLLING);
  if (touches) {
    // Remember the original touch points so we can use later calls to check for swipes
    GTPoint points[touches];
    memcpy(points, ts.getPoints(), touches * sizeof(GTPoint));
    // If menu press
    if (!uiHeader->handleTouch(touches, points, []() -> bool {
      return ts.touched(GT911_MODE_POLLING);
    })) {
      // Attempt to detect swipe
      UI::Swipe swipe = UI::Swipe::NONE;
      delay(5);
      uint32_t timeout = millis() + 150;
      while (millis() < timeout && ts.touched(GT911_MODE_POLLING)) {
        GTPoint p = ts.getPoint(0);
        if (points[0].y - p.y > GESTURE_DISTANCE) {
          swipe = UI::Swipe::UP;
        } else if (p.y - points[0].y > GESTURE_DISTANCE) {
          swipe = UI::Swipe::DOWN;
        } else if (points[0].x - p.x > GESTURE_DISTANCE) {
          swipe = UI::Swipe::LEFT;
        } else if (p.x - points[0].x > GESTURE_DISTANCE) {
          swipe = UI::Swipe::RIGHT;
        }
        if (swipe != UI::Swipe::NONE) {
          while (ts.touched(GT911_MODE_POLLING)) {
            delay(10);
          }
          break;
        }
      }

      if (swipe == UI::Swipe::NONE) {
        activeUI->handleTouch(touches, points, []() -> bool {
          return ts.touched(GT911_MODE_POLLING);
        });
        Serial.println(ESP.getFreeHeap());
      } else {
        activeUI->swipe(swipe);
      }
    }
  } else if (encoder->read() >= 4) {
    activeUI->encoderRotate(UI::Encoder::Rotation::CW);
    encoder->write(0);
  } else if (encoder->read() <= -4) {
    activeUI->encoderRotate(UI::Encoder::Rotation::CCW);
    encoder->write(0);
  } else if (encoderBtnState == UI::Encoder::ButtonState::PRESSED && millis() - encoderPressMillis > 2000) { // Encoder pressed and held for more than 2 seconds
    encoderBtnState = UI::Encoder::ButtonState::IDLE;
    dccExCS.emergencyStopAll(); // Stop all locos
    activeUI->encoderPress(UI::Encoder::ButtonPress::LONG);
  } else if (encoderBtnState == UI::Encoder::ButtonState::RELEASED && millis() - encoderPressMillis < 1000) { // Encoder pressed for less than 1 second
    encoderBtnState = UI::Encoder::ButtonState::IDLE;
    activeUI->encoderPress(UI::Encoder::ButtonPress::SHORT);
  }
  #if defined(USE_ACCELEROMETER)
  else if (Settings.rotation == SettingsClass::Rotation::ACCELEROMETER && activeUI != nullptr) {
    DFRobot_LIS2DW12::eOrient_t orientation = acce.getOrientation();
    if (orientation != lastOrientation && (orientation == DFRobot_LIS2DW12::eYDown || orientation == DFRobot_LIS2DW12::eYUp)) {
      lastOrientation = orientation;
      setRotation();
    }
  }
  #endif

  while (!UI::tasks.empty()) {
    UI::tasks[0]();
    UI::tasks.pop_front();
  }

  activeUI->loop();

  delay(1);
}
