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
    
    uint16_t _updatedHandler;

    Label *_labelSSID;
    Label *_labelPassword;
    Label *_labelServer;
    Label *_labelPort;
    Label *_labelScan;

    bool _alternateQR = true;

    void drawQR();
  public:
    WiFiUI(TFT_eSPI *tft, Tasks *tasks);
    ~WiFiUI();

    void loop();
    void rotated();
    void swipe(Swipe swipe = Swipe::NONE);
    void updated();
};

#endif
