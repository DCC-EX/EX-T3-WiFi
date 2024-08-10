#ifndef WIFI_UI_H
#define WIFI_UI_H

#include <UI.h>
#include <DNSServer.h>
#include <ThrottleServer.h>
#include <Elements/Label.h>

class WiFiUI : public UI {
  private:
    DNSServer dns;
    ThrottleServer server;

    wifi_event_id_t _ipGotHandler;
    wifi_event_id_t _ipDisconnectedHandler;
    uint16_t _updatedHandler;

    Label* _labelSSID;
    Label* _labelPassword;
    Label* _labelIP;
    Label* _labelServer;
    Label* _labelPort;
    Label* _labelScan;

    bool _alternateQR = true;

    void drawQR();
    void keyboard(const String& title, const String &value, void(*setting)(const String&));
    void keypad(const String& title, uint16_t value, void(*setting)(uint16_t));
  public:
    WiFiUI();
    ~WiFiUI();

    void loop();
    void redraw();
    
    bool swipe(Swipe swipe = Swipe::NONE);
    void updated(bool redraw = true);
};

#endif
