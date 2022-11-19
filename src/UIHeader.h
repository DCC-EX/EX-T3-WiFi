#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <TFT_eSPI.h>
#include <Touch.h>
#include <Elements/Label.h>

class UIHeader {
  private:
    enum class PowerStatus : uint8_t {
      STATUS_CHARGING,
      STATUS_RECHARGE,
      STATUS_LOW,
      STATUS_MEDIUM,
      STATUS_HIGH
    };

    TFT_eSPI *_tft;
    std::unique_ptr<Touch> _menu;
    std::unique_ptr<Label> _locos;
    
    PowerStatus _powerStatus = PowerStatus::STATUS_HIGH;
    bool _wifiStatus = false;
    bool _csStatus = false;
    uint8_t _locoCount = 0;

    void updatePowerStatus();
    void updateWiFiStatus();
    void updateCSStatus();
    void updateLocoCount();
  public:
    UIHeader(TFT_eSPI *tft);

    void draw();
    bool menuTouch(uint16_t x, uint16_t y);
    void setPowerStatus(float voltage);
    void setWiFiStatus(bool connected);
    void setCSStatus(bool connected);
    void setLocoCount(uint8_t count);
};

#endif
