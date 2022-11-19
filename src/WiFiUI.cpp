#include <WiFiUI.h>
#include <Elements/Header.h>
#include <WiFi.h>
#include <Settings.h>
#include <qrcode_espi.h>

WiFiUI::WiFiUI(TFT_eSPI *tft, Tasks *tasks) : UI(tft, tasks) {
  _elements.reserve(11);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(THROTTLE_AP_NAME, THROTTLE_AP_PWD);
  
  dns.start(53, THROTTLE_AP_NAME, WiFi.softAPIP());
  server.begin();

  _updatedHandler = Settings.addEventListener(static_cast<uint8_t>(SettingsClass::Event::CS_CHANGE), std::bind(&WiFiUI::updated, this));

  addElement<Header>(0, 40, 320, 18, "CS Settings");

  _labelSSID = addElement<Label>(0, 67, 320, 18, "", true);
  _labelPassword = addElement<Label>(0, 94, 320, 18, "", true);
  _labelServer = addElement<Label>(0, 121, 320, 18, "", true);
  _labelPort = addElement<Label>(0, 148, 320, 18, "", true);

  addElement<Header>(0, 175, 320, 18, "Throttle AP Settings");

  addElement<Label>(0, 202, 320, 18, "SSID: " THROTTLE_AP_NAME, true);
  addElement<Label>(0, 229, 320, 18, "Password: " THROTTLE_AP_PWD, true);
  addElement<Label>(0, 256, 320, 18, "http://" THROTTLE_AP_NAME, true);

  _labelScan = addElement<Label>(0, 283, 320, 18, "", true);

  addElement<Label>(0, 460, 320, 18, "Swipe left/right for AP/http:// QR code", true);

  updated();
  swipe();
}

WiFiUI::~WiFiUI() {
  WiFi.mode(WIFI_STA);

  dns.stop();
  server.end();

  Settings.removeEventListener(static_cast<uint8_t>(SettingsClass::Event::CS_CHANGE), _updatedHandler);
}

void WiFiUI::loop() {
  dns.processNextRequest();
}

void WiFiUI::rotated() {
  UI::rotated();
  drawQR();
}

void WiFiUI::drawQR() {
  TFT_eSprite qr(_tft);
  qr.createSprite(132, 132);
  QRcode_eSPI qrcode(&qr);
  qrcode.init();
  if (_alternateQR) {
    // https://en.wikipedia.org/wiki/QR_code#Joining_a_Wi%E2%80%91Fi_network
    // WIFI:S:MySSID;T:WPA;P:MyPassW0rd;;
    qrcode.create("WIFI:S:" THROTTLE_AP_NAME ";T:WPA;P:" THROTTLE_AP_PWD ";;");
  } else {
    qrcode.create("http://" THROTTLE_AP_NAME);
  }
  qr.pushSprite(94, 312);
}

void WiFiUI::swipe(Swipe swipe) {
  if (swipe == Swipe::NONE || swipe == Swipe::LEFT || swipe == Swipe::RIGHT) {
    if (_alternateQR) {
      _labelScan->setLabel("Scan below to connect to throttle AP");
    } else {
      _labelScan->setLabel("Scan below to connect to throttle setup");
    }
    drawQR();
    if (swipe != Swipe::NONE) {
      _alternateQR = !_alternateQR;
    }
  }
}

void WiFiUI::updated() {
  _tasks->push_back([this]() {
    String SSID("SSID: ");
    SSID += Settings.CS.SSID().isEmpty() ? "(Not Set)" : Settings.CS.SSID();
    _labelSSID->setLabel(SSID);

    String password("Password: ");
    password += Settings.CS.password().isEmpty() ? "(Not Set)" : Settings.CS.password();
    _labelPassword->setLabel(password);

    String server("Server: ");
    server += Settings.CS.server().isEmpty() ? "(Not Set)" : Settings.CS.server();
    _labelServer->setLabel(server);

    String port("Port: ");
    port += Settings.CS.port() == 0 ? "(Not Set)" : String(Settings.CS.port());
    _labelPort->setLabel(port);
  });
}
