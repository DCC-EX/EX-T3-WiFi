#ifndef LOCOS_H
#define LOCOS_H

#include <Events.h>
#include <vector>

class Locos : public Events {
  public:
    enum class Event : uint8_t {
      COUNT_CHANGE
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
