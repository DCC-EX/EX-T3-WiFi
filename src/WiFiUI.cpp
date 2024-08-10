#include <WiFiUI.h>
#include <Children/Keyboard.h>
#include <Children/Keypad.h>
#include <Elements/Header.h>
#include <WiFi.h>
#include <Settings.h>
#include <qrcode_espi.h>

WiFiUI::WiFiUI() {
  _elements.reserve(13);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(Settings.AP.SSID.c_str(), Settings.AP.password.c_str());
  
  dns.start(53, Settings.AP.SSID.c_str(), WiFi.softAPIP());
  server.begin();

  _ipGotHandler = WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
    _tasks.push_back([this] {
      _labelIP->setLabel(String("IP: ") += WiFi.localIP().toString());
    });
  }, ARDUINO_EVENT_WIFI_STA_GOT_IP);

  _ipDisconnectedHandler = WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
    _tasks.push_back([this] {
      _labelIP->setLabel("IP: Not Connected");
    });
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  _updatedHandler = Settings.addEventListener(SettingsClass::Event::CS_CHANGE, [this](void* parameter) {
    updated(parameter == nullptr && _child == nullptr);
  });

  addElement<Header>(0, 40, 320, 18, "WiFi Settings");

  _labelSSID = addElement<Label>(0, 67, 320, 18, "", true);
  _labelSSID->onRelease([this](void*) {
    keyboard("SSID", Settings.CS.SSID(), [](const String &value) {
      Settings.CS.SSID(value);
    });
  });

  _labelPassword = addElement<Label>(0, 94, 320, 18, "", true);
  _labelPassword->onRelease([this](void*) {
    keyboard("Password", "", [](const String &value) {
      Settings.CS.password(value);
    });
  });

  _labelIP = addElement<Label>(0, 121, 320, 18, "IP: Not Connected", true);
  if (WiFi.isConnected()) {
    _labelIP->setLabel(String("IP: ") += WiFi.localIP().toString());
  }

  addElement<Header>(0, 148, 320, 18, "CS Settings");

  _labelServer = addElement<Label>(0, 175, 320, 18, "", true);
  _labelServer->onRelease([this](void*) {
    keyboard("Server", Settings.CS.server(), [](const String &value) {
      Settings.CS.server(value);
    });
  });

  _labelPort = addElement<Label>(0, 202, 320, 18, "", true);
  _labelPort->onRelease([this](void*) {
    keypad("Port", Settings.CS.port(), [](uint16_t value) {
      Settings.CS.port(value);
    });
  });

  addElement<Header>(0, 229, 320, 18, "Throttle AP Settings");

  addElement<Label>(0, 256, 320, 18, String("SSID: ") += Settings.AP.SSID, true);
  addElement<Label>(0, 283, 320, 18, String("Password: ") += Settings.AP.password, true);
  addElement<Label>(0, 310, 320, 18, String("http://") += Settings.AP.SSID, true);

  _labelScan = addElement<Label>(0, 337, 320, 18, "", true);

  addElement<Label>(0, 460, 320, 18, "Swipe left/right for AP/http:// QR code", true);

  updated();
  swipe();
}

WiFiUI::~WiFiUI() {
  WiFi.mode(WIFI_STA);

  dns.stop();
  server.end();

  WiFi.removeEvent(_ipGotHandler);
  WiFi.removeEvent(_ipDisconnectedHandler);
  Settings.removeEventListener(SettingsClass::Event::CS_CHANGE, _updatedHandler);
}

void WiFiUI::loop() {
  dns.processNextRequest();
}

void WiFiUI::redraw() {
  drawQR();
}

void WiFiUI::drawQR() {
  TFT_eSprite qr(UI::tft);
  qr.createSprite(76, 76);
  QRcode_eSPI qrcode(&qr);
  qrcode.init();
  if (_alternateQR) {
    // https://en.wikipedia.org/wiki/QR_code#Joining_a_Wi%E2%80%91Fi_network
    // WIFI:S:MySSID;T:WPA;P:MyPassW0rd;;
    String SSID("WIFI:S:");
    SSID += Settings.AP.SSID;
    SSID += ";T:WPA;P:";
    SSID += Settings.AP.password;
    SSID += ";;";
    qrcode.create(SSID);
  } else {
    qrcode.create(String("http://") += Settings.AP.SSID);
  }
  qr.pushSprite(122, 369);
}

bool WiFiUI::swipe(Swipe swipe) {
  if (swipe == Swipe::NONE || swipe == Swipe::LEFT || swipe == Swipe::RIGHT) {
    if (swipe != Swipe::NONE) {
      _alternateQR = !_alternateQR;
    }
    if (_alternateQR) {
      _labelScan->setLabel("Scan below to connect to throttle AP");
    } else {
      _labelScan->setLabel("Scan below to connect to throttle setup");
    }
    drawQR();
  }

  return true;
}

void WiFiUI::updated(bool redraw) {
  auto update = [this, redraw] {
    String SSID("SSID: ");
    SSID += Settings.CS.SSID().isEmpty() ? "(Not Set)" : Settings.CS.SSID();
    _labelSSID->setLabel(SSID, redraw);

    String password("Password: ");
    password += Settings.CS.password().isEmpty() ? "(Not Set)" : Settings.CS.password();
    _labelPassword->setLabel(password, redraw);

    String server("Server: ");
    server += Settings.CS.server().isEmpty() ? "(Not Set)" : Settings.CS.server();
    _labelServer->setLabel(server, redraw);

    String port("Port: ");
    port += Settings.CS.port() == 0 ? "(Not Set)" : String(Settings.CS.port());
    _labelPort->setLabel(port, redraw);
  };

  if (redraw) {
    _tasks.push_back(update);
  } else {
    update();
  }
}

void WiFiUI::keyboard(const String& title, const String &value, void(*setting)(const String&)) {
  _tasks.push_back([this, title, value, setting] {
    auto keyboard = setChild<Keyboard>(title, value);
    keyboard->addEventListener(Keyboard::Event::ENTER, [this, setting](void* parameter) {
      setting(*static_cast<String*>(parameter));
      if (Settings.CS.valid()) {
        Settings.save();
        Settings.dispatchEvent(SettingsClass::Event::CS_CHANGE, reinterpret_cast<void*>(1));
      }
      _tasks.push_back([this] {
        reset(true);
      });
    });
    keyboard->addEventListener(Keyboard::Event::CANCEL, [this](void*) {
      _tasks.push_back([this] {
        reset(true);
      });
    });
  });
}

void WiFiUI::keypad(const String& title, uint16_t value, void(*setting)(uint16_t)) {
  _tasks.push_back([this, title, value, setting] {
    auto keypad = setChild<Keypad>(title, 65535, 1, value);
    keypad->addEventListener(Keypad::Event::ENTER, [this, setting](void* parameter) {
      setting(*static_cast<uint16_t*>(parameter));
      if (Settings.CS.valid()) {
        Settings.save();
        Settings.dispatchEvent(SettingsClass::Event::CS_CHANGE, reinterpret_cast<void*>(1));
      }
      _tasks.push_back([this] {
        reset(true);
      });
    });
    keypad->addEventListener(Keypad::Event::CANCEL, [this](void*) {
      _tasks.push_back([this] {
        reset(true);
      });
    });
  });
}
