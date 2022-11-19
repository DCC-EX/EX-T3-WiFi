#include <UIHeader.h>
#include <Img/BMP.h>

UIHeader::UIHeader(TFT_eSPI *tft) : _tft(tft) {
  _menu = std::make_unique<Touch>(290, 0, 30, 30);
  _locos = std::make_unique<Label>(_tft, 180, 10, 100, 18);
}

void UIHeader::draw() {
  updatePowerStatus();
  updateWiFiStatus();
  updateCSStatus();

  BMP(SPIFFS.open("/icons/conductor.bmp"), _tft, 150, 0);
  updateLocoCount();

  BMP(SPIFFS.open("/icons/menu.bmp"), _tft, 290, 0);
}

bool UIHeader::menuTouch(uint16_t x, uint16_t y) {
  return _menu->contains(x, y);
}

void UIHeader::updatePowerStatus() {
  if (_powerStatus == PowerStatus::STATUS_CHARGING) { // If we can detect charging in the future
    BMP(SPIFFS.open("/icons/battery-charging.bmp"), _tft, 0, 2);
  } else if (_powerStatus == PowerStatus::STATUS_HIGH) {
    BMP(SPIFFS.open("/icons/battery-full.bmp"), _tft, 0, 2);
  } else if (_powerStatus == PowerStatus::STATUS_MEDIUM) {
    BMP(SPIFFS.open("/icons/battery-medium.bmp"), _tft, 0, 2);
  } else if (_powerStatus == PowerStatus::STATUS_LOW) {
    BMP(SPIFFS.open("/icons/battery-low.bmp"), _tft, 0, 2);
  } else if (_powerStatus == PowerStatus::STATUS_RECHARGE) {
    BMP(SPIFFS.open("/icons/battery-recharge.bmp"), _tft, 0, 2);
  }
}

void UIHeader::setPowerStatus(float voltage) {
  if (voltage > 4.0 && _powerStatus != PowerStatus::STATUS_HIGH) {
    _powerStatus = PowerStatus::STATUS_HIGH;
    updatePowerStatus();
  } else if (voltage > 3.85 && voltage <= 3.99 && _powerStatus != PowerStatus::STATUS_MEDIUM) {
    _powerStatus = PowerStatus::STATUS_MEDIUM;
    updatePowerStatus();
  } else if (voltage > 3.75 && voltage <= 3.84 && _powerStatus != PowerStatus::STATUS_LOW) {
    _powerStatus = PowerStatus::STATUS_LOW;
    updatePowerStatus();
  } else if (voltage < 3.74 && _powerStatus != PowerStatus::STATUS_RECHARGE) {
    _powerStatus = PowerStatus::STATUS_RECHARGE;
    updatePowerStatus();
  }
}

void UIHeader::updateWiFiStatus() {
  BMP(SPIFFS.open(_wifiStatus
      ? "/icons/wifi-connected.bmp"
      : "/icons/wifi-disconnected.bmp")
    , _tft, 70, 0);
}

void UIHeader::setWiFiStatus(bool connected) {
  if (_wifiStatus != connected) {
    _wifiStatus = connected;
    updateWiFiStatus();
  }
}

void UIHeader::updateCSStatus() {
  BMP(SPIFFS.open(_csStatus
      ? "/icons/cs-connected.bmp"
      : "/icons/cs-disconnected.bmp")
    , _tft, 110, 0);
}

void UIHeader::setCSStatus(bool connected) {
  if (_csStatus != connected) {
    _csStatus = connected;
    updateCSStatus();
  }
}

void UIHeader::updateLocoCount() {
  String label(": ");
  label += _locoCount;
  _locos->setLabel(label);
}

void UIHeader::setLocoCount(uint8_t count) {
  if (_locoCount != count) {
    _locoCount = count;
    updateLocoCount();
  }
}
