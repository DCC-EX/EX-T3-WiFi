#ifndef EVENTS_H
#define EVENTS_H

#include <functional>
#include <map>
#include <vector>

class Events {
  public:
    using EventCallback = std::function<void(void* parameter)>;
  private:
    struct EventHandler {
      EventCallback cb;
      uint16_t id;
    };
    std::map<uint8_t, std::vector<EventHandler>> _events;
    uint16_t _count = 0;
  public:
    uint16_t addEventListener(uint8_t event, const EventCallback&& callback);
    void removeEventListener(uint8_t event, uint16_t handler);
    void dispatchEvent(uint8_t event, void* parameter = nullptr);
};

#endif
