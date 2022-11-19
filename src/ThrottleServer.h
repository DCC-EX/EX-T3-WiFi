#ifndef SERVER_H
#define SERVER_H

#include <ESPAsyncWebServer.h>

class ThrottleServer : public AsyncWebServer {
  public:
    ThrottleServer();

    void begin();
};

#endif
