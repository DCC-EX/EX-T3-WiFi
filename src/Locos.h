#ifndef LOCOS_H
#define LOCOS_H

#include <Events.h>
#include <vector>

class Locos : public Events {
  public:
    struct Event {
      static constexpr uint8_t COUNT_CHANGE = 0;
    };
  private:
    std::vector<uint16_t> _locos;
    uint8_t _active = 0;

    void change();
  public:
    Locos();

    operator uint16_t ();

    void add(uint16_t address);
    uint16_t remove();
    void next();
    void prev();
};

#endif
