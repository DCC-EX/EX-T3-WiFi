#include <Events.h>
#include <algorithm>

uint16_t Events::addEventListener(uint8_t event, const EventCallback &&callback) {
  return _events[event].emplace_back(EventHandler { callback, _count++ }).id;
}

void Events::removeEventListener(uint8_t event, uint16_t handler) {
  _events[event].erase(
    std::remove_if(
      _events[event].begin(),
      _events[event].end(),
      [handler](EventHandler cb) { return cb.id == handler; }
    ),
    _events[event].end()
  );
}

void Events::dispatchEvent(uint8_t event, void *parameter) {
  for (const auto &handler : _events[event]) {
    handler.cb(parameter);
  }
}
