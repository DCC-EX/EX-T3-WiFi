#include <DCCExCS.h>
#include <Functions.h>

DCCExCS::DCCExCS(AsyncClient& csClient) : _csClient(csClient) {
  _timeout = xTimerCreate("timeout", pdMS_TO_TICKS(5000), pdFALSE, this, [](TimerHandle_t handle) {
    static_cast<DCCExCS*>(pvTimerGetTimerID(handle))->dispatchEvent(Event::TIMEOUT);
  });

  addEventListener(Event::TIMEOUT, [this](void*) {
    _response.matched = NULL;
  });
}

void DCCExCS::createTimeout() {
  xTimerStart(_timeout, 0);
}

void DCCExCS::deleteTimeout() {
  xTimerStop(_timeout, 0);
}

void DCCExCS::handleCS(uint8_t* data, uint16_t size) {
  std::string str(reinterpret_cast<char*>(data), size);
  std::smatch matches;
  
  if (std::regex_search(str, matches, std::regex("l (\\d+) -?\\d+ (\\d+) (\\d+)"))) { // Loco broadcast
    uint16_t address = fromChars<uint16_t>(matches.str(1));
    uint8_t speedCode = fromChars<uint8_t>(matches.str(2));
    uint32_t functions = fromChars<uint32_t>(matches.str(3));

    Loco loco(address, speedCode, functions);
    dispatchEvent(Event::BROADCAST_LOCO, &loco);
  } else if (std::regex_search(str, matches, std::regex("p(\\d)\\s?(\\w+)?"))) { // Power broadcast
    Power power;

    if (matches.str(1)[0] == '1') {
      if (!matches[2].matched) {
        power.main = true;
        power.prog = true;
      } else {
        if (matches.str(2).compare("MAIN") == 0) {
          power.main = true;
        } else if (matches.str(2).compare("PROG") == 0) {
          power.prog = true;
        } else if (matches.str(2).compare("JOIN") == 0) {
          power.main = true;
          power.prog = true;
          power.join = true;
        }
      }
    }

    dispatchEvent(Event::BROADCAST_POWER, &power);
  } else if (std::regex_search(str, matches, std::regex("iDCC-EX V-([^\\s]+) \\/ ([^\\s]+) \\/ ([^\\s]+) (.+)"))) { // Version response
    Version version(matches.str(1).c_str(), matches.str(2).c_str(), matches.str(3).c_str(), matches.str(4).c_str());
    dispatchEvent(Event::VERSION, &version);
  } else if (_response.matched && std::regex_search(str, matches, std::regex(_response.match))) { // Response to prog
    deleteTimeout();
    _response.matched(matches);
    _response.matched = NULL;
  }
}

void DCCExCS::getCSVersion() {
  _csClient.write("<s>\n");
}

void DCCExCS::getCSPower() {
  _csClient.write("<U p>\n");
}

void DCCExCS::setCSPower(uint8_t power, bool state) {
  const char* cmd = "";
  if (power == Power::MAIN) {
    cmd = " MAIN";
  } else if (power == Power::PROG) {
    cmd = " PROG";
  } else if (power == Power::JOIN) {
    cmd = " JOIN";
  }

  char buf[10];
  sprintf(buf, "<%d%s>\n", state ? 1 : 0, cmd);
  _csClient.write(buf);
}

void DCCExCS::emergencyStopAll() {
  _csClient.write("<!>\n");
}

void DCCExCS::acquireLoco(uint16_t address) {
  char buf[12];
  sprintf(buf, "<t %d>\n", address);
  _csClient.write(buf);
}

void DCCExCS::releaseLoco(uint16_t address) {
  char buf[12];
  sprintf(buf, "<- %d>\n", address);
  _csClient.write(buf);
}

void DCCExCS::setLocoThrottle(uint16_t address, int8_t speed, uint8_t direction) {
  char buf[20];
  sprintf(buf, "<t %d %d %d>\n", address, speed, direction);
  _csClient.write(buf);
}

void DCCExCS::setLocoFn(uint16_t address, uint8_t fn, bool state) {
  char buf[18];
  sprintf(buf, "<F %d %d %d>\n", address, fn, state);
  _csClient.write(buf);
}

void DCCExCS::getLocoAddress() {
  _response = {
    "r (-?\\d+)",
    [this](std::smatch &matches) {
      int16_t result = fromChars<int16_t>(matches.str(1));
      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  _csClient.write("<R>\n");

  createTimeout();
}

void DCCExCS::setLocoAddress(uint16_t address) {
  _response = {
    "w (-?\\d+)",
    [this, address](std::smatch &matches) {
      int16_t result = -1;
      if (fromChars<int16_t>(matches.str(1)) == address) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[12];
  sprintf(buf, "<W %d>\n", address);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::getLocoCVByte(uint16_t cv) {
  _response = {
    "r12345\\|32767\\|(\\d+) (-?\\d+)",
    [this, cv](std::smatch &matches) {
      int16_t result = -1;
      if (fromChars<uint16_t>(matches.str(1)) == cv) {
        result = fromChars<int16_t>(matches.str(2));
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  char buf[22];
  sprintf(buf, "<R %d 12345 32767>\n", cv);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::setLocoCVByte(uint16_t cv, uint8_t value) {
  _response = {
    "r (\\d+) (-?\\d+)",
    [this, cv, value](std::smatch &matches) {
      int16_t result = -1;
      if (fromChars<uint16_t>(matches.str(1)) == cv &&
          fromChars<uint8_t>(matches.str(2)) == value) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[14];
  sprintf(buf, "<W %d %d>\n", cv, value);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::getLocoCVBit(uint16_t cv, uint8_t bit) {
  _response = {
    "r12345\\|32767\\|(\\d+) (-?\\d+)",
    [this, cv, bit](std::smatch &matches) {
      int16_t result = -1;
      if (fromChars<uint16_t>(matches.str(1)) == cv) {
        uint8_t value = fromChars<uint8_t>(matches.str(2));
        result = (value >> bit) & 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  char buf[22];
  sprintf(buf, "<R %d 12345 32767>\n", cv);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::setLocoCVBit(uint16_t cv, uint8_t bit, bool value) {
  _response = {
    "r12345\\|32767\\|(\\d+) (\\d+) (-?\\d+)",
    [this, cv, bit, value](std::smatch &matches) {
      int16_t result = -1;
      if (fromChars<uint16_t>(matches.str(1)) == cv &&
          fromChars<uint8_t>(matches.str(2)) == bit &&
          fromChars<uint8_t>(matches.str(3)) == value) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[26];
  sprintf(buf, "<B %d %d %d 12345 32767>\n", cv, bit, value);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::accessory(uint16_t address, bool state) {
  char buf[12];
  sprintf(buf, "<a %d %d>\n", address, state ? 1 : 0);
  _csClient.write(buf);
}

void DCCExCS::setAckLimit(uint16_t limit) {
  char buf[19];
  sprintf(buf, "<D ACK LIMIT %d>\n", limit);
  _csClient.write(buf);
}

void DCCExCS::setAckMin(uint16_t min) {
  char buf[19];
  sprintf(buf, "<D ACK MIN %d>\n", min);
  _csClient.write(buf);
}

void DCCExCS::setAckMax(uint16_t max) {
  char buf[19];
  sprintf(buf, "<D ACK MAX %d>\n", max);
  _csClient.write(buf);
}
