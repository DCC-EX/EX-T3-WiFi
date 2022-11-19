#ifndef DCC_EX_CS_H
#define DCC_EX_CS_H

#include <Arduino.h>
#include <bitset>
#include <regex>
#include <Events.h>
#include <AsyncTCP.h>

class DCCExCS : public Events {
  private:
    AsyncClient *_csClient;
    TaskHandle_t _timeout;

    struct {
      const char *match;
      std::function<void (std::smatch &matches)> matched;
    } _response;
    
    void createTimeout();
    void deleteTimeout();
  public:
    enum class Event : uint8_t {
      TIMEOUT,
      BROADCAST_LOCO,
      BROADCAST_POWER,
      PROGRAM_READ,
      PROGRAM_WRITE
    };

    struct Power {
      enum : uint8_t {
        ALL,
        MAIN,
        PROG,
        JOIN
      };

      bool main = false;
      bool prog = false;
      bool join = false;

      Power() { }
      Power(const Power& power) : main(power.main), prog(power.prog), join(power.join) { }
    };

    struct Loco {
      uint16_t address;
      uint8_t speed;
      uint8_t direction;
      std::bitset<32> functions;
      Loco(uint16_t address)
          : address(address), speed(UINT8_MAX), direction(UINT8_MAX), functions(UINT32_MAX) { }
      Loco(uint16_t address, uint8_t speed, bool direction, uint32_t functions)
          : address(address), speed(speed), direction(direction), functions(functions) { }
      Loco(uint16_t address, uint8_t speedCode, uint32_t functions)
          : address(address), speed(max(0, (speedCode & 0x7F) - 1)),
            direction((speedCode & 0x80) == 128), functions(functions) { }
    };

    DCCExCS(AsyncClient *csClient);

    void handleCS(uint8_t *data, uint16_t size);
    void setCSPower(uint8_t power, bool state);
    void getCSPower();
    void emergencyStopAll();
    void acquireLoco(uint16_t address);
    void releaseLoco(uint16_t address);
    void setLocoThrottle(uint16_t address, int8_t speed, uint8_t direction);
    void setLocoFn(uint16_t address, uint8_t fn, bool state);
    void writeLocoAddress(uint16_t address);
    void readLocoAddress();
    void writeLocoCVByte(uint16_t cv, uint8_t value);
    void readLocoCVByte(uint16_t cv);
    void writeLocoCVBit(uint16_t cv, uint8_t bit, bool value);
    void setAckLimit(uint16_t limit);
    void setAckMin(uint16_t min);
    void setAckMax(uint16_t max);
    void accessory(uint16_t address, bool state);
};

#endif
