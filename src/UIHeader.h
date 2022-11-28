#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <UI.h>
#include <Events.h>
#include <Elements/Image.h>
#include <Elements/Button.h>
#include <Elements/Label.h>

class UIHeader : public UI, public Events {
  public:
    enum class Event : uint8_t {
      MENU
    };
  private:
    enum class PowerStatus : uint8_t {
      STATUS_CHARGING,
      STATUS_RECHARGE,
      STATUS_LOW,
      STATUS_MEDIUM,
      STATUS_HIGH
    };

    Image* _power;
    Image* _wifi;
    Image* _cs;
    Label* _locos;
    
    PowerStatus _powerStatus = PowerStatus::STATUS_HIGH;
    bool _wifiStatus = false;
    bool _csStatus = false;
    uint8_t _locoCount = 0;

    void updatePowerStatus();
    void updateWiFiStatus();
    void updateCSStatus();
    void updateLocoCount();
  public:
    UIHeader();

    void setPowerStatus(float voltage);
    void setWiFiStatus(bool connected);
    void setCSStatus(bool connected);
    void setLocoCount(uint8_t count);
};

#endif
