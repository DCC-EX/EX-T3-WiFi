#include <DCCExCS.h>

DCCExCS::DCCExCS(AsyncClient& csClient) : _csClient(csClient) {
  addEventListener(Event::TIMEOUT, [this](void* parameter) {
    _response.matched = NULL;
  });
}

void DCCExCS::createTimeout() {
  xTaskCreatePinnedToCore([](void* parameter) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    static_cast<DCCExCS*>(parameter)->dispatchEvent(Event::TIMEOUT);
    vTaskDelete(NULL);
  }, "timeout", 1024 * 4, this, 1, &_timeout, 1);
}

void DCCExCS::deleteTimeout() {
  vTaskDelete(_timeout);
}

void DCCExCS::handleCS(uint8_t* data, uint16_t size) {
  std::string str(reinterpret_cast<char*>(data), size);
  std::smatch matches;

  if (std::regex_search(str, matches, std::regex("l (\\d+) -?\\d (\\d+) (\\d+)"))) { // Loco broadcast
    Loco loco(
      strtoul(matches[1].str().c_str(), (char**)NULL, 10),
      strtol(matches[2].str().c_str(), (char**)NULL, 10),
      strtoul(matches[3].str().c_str(), (char**)NULL, 10)
    );
    dispatchEvent(Event::BROADCAST_LOCO, &loco);
  } else if (std::regex_search(str, matches, std::regex("p(\\d)\\s?(\\w+)?"))) { // Power broadcast
    Power power;

    if (matches[1].str()[0] == '1') {
      if (!matches[2].matched) {
        power.main = true;
        power.prog = true;
      } else {
        if (matches[2].str().compare("MAIN") == 0) {
          power.main = true;
        } else if (matches[2].str().compare("PROG") == 0) {
          power.prog = true;
        } else if (matches[2].str().compare("JOIN") == 0) {
          power.main = true;
          power.prog = true;
          power.join = true;
        }
      }
    }

    dispatchEvent(Event::BROADCAST_POWER, &power);
  } else if (_response.matched && std::regex_search(str, matches, std::regex(_response.match))) { // Response to prog
    deleteTimeout();
    _response.matched(matches);
    _response.matched = NULL;
  }
}

void DCCExCS::getCSPower() {
  _csClient.write("<U p>");
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

  char buf[9];
  sprintf(buf, "<%d%s>", state ? 1 : 0, cmd);
  _csClient.write(buf);
}

void DCCExCS::emergencyStopAll() {
  _csClient.write("<!>");
}

void DCCExCS::acquireLoco(uint16_t address) {
  char buf[11];
  sprintf(buf, "<t %d>", address);
  _csClient.write(buf);
}

void DCCExCS::releaseLoco(uint16_t address) {
  char buf[11];
  sprintf(buf, "<- %d>", address);
  _csClient.write(buf);
}

void DCCExCS::setLocoThrottle(uint16_t address, int8_t speed, uint8_t direction) {
  char buf[19];
  sprintf(buf, "<t %d %d %d>", address, speed, direction);
  _csClient.write(buf);
}

void DCCExCS::setLocoFn(uint16_t address, uint8_t fn, bool state) {
  char buf[17];
  sprintf(buf, "<F %d %d %d>", address, fn, state);
  _csClient.write(buf);
}

void DCCExCS::getLocoAddress() {
  _response = {
    "r (-?\\d+)",
    [this](std::smatch &matches) {
      int16_t result = strtol(matches[1].str().c_str(), (char**)NULL, 10);
      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  _csClient.write("<R>");

  createTimeout();
}

void DCCExCS::setLocoAddress(uint16_t address) {
  _response = {
    "w (-?\\d+)",
    [this, address](std::smatch &matches) {
      int16_t result = -1;
      if (strtoul(matches[1].str().c_str(), (char**)NULL, 10) == address) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[11];
  sprintf(buf, "<W %d>", address);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::getLocoCVByte(uint16_t cv) {
  _response = {
    "r12345\\|32767\\|(\\d+) (-?\\d+)",
    [this, cv](std::smatch &matches) {
      int16_t result = -1;
      if (strtoul(matches[1].str().c_str(), (char**)NULL, 10) == cv) {
        result = strtol(matches[2].str().c_str(), (char**)NULL, 10);
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  char buf[21];
  sprintf(buf, "<R %d 12345 32767>", cv);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::setLocoCVByte(uint16_t cv, uint8_t value) {
  _response = {
    "r (\\d+) (-?\\d+)",
    [this, cv, value](std::smatch &matches) {
      int16_t result = -1;
      if (strtoul(matches[1].str().c_str(), (char**)NULL, 10) == cv &&
          strtoul(matches[2].str().c_str(), (char**)NULL, 10) == value) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[13];
  sprintf(buf, "<W %d %d>", cv, value);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::getLocoCVBit(uint16_t cv, uint8_t bit) {
  _response = {
    "r12345\\|32767\\|(\\d+) (-?\\d+)",
    [this, cv, bit](std::smatch &matches) {
      int16_t result = -1;
      if (strtoul(matches[1].str().c_str(), (char**)NULL, 10) == cv) {
        uint8_t value = strtol(matches[2].str().c_str(), (char**)NULL, 10);
        result = (value >> bit) & 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_READ, &result);
    }
  };

  char buf[21];
  sprintf(buf, "<R %d 12345 32767>", cv);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::setLocoCVBit(uint16_t cv, uint8_t bit, bool value) {
  _response = {
    "r12345\\|32767\\|(\\d+) (\\d+) (-?\\d+)",
    [this, cv, bit, value](std::smatch &matches) {
      int16_t result = -1;
      if (strtoul(matches[1].str().c_str(), (char**)NULL, 10) == cv &&
          strtoul(matches[2].str().c_str(), (char**)NULL, 10) == bit &&
          strtoul(matches[3].str().c_str(), (char**)NULL, 10) == value) {
        result = 1;
      }

      dispatchEvent(DCCExCS::Event::PROGRAM_WRITE, &result);
    }
  };

  char buf[25];
  sprintf(buf, "<B %d %d %d 12345 32767>", cv, bit, value);
  _csClient.write(buf);

  createTimeout();
}

void DCCExCS::accessory(uint16_t address, bool state) {
  char buf[11];
  sprintf(buf, "<a %d %d>", address, state ? 1 : 0);
  _csClient.write(buf);
}

void DCCExCS::setAckLimit(uint16_t limit) {
  char buf[18];
  sprintf(buf, "<D ACK LIMIT %d>", limit);
  _csClient.write(buf);
}

void DCCExCS::setAckMin(uint16_t min) {
  char buf[18];
  sprintf(buf, "<D ACK MIN %d>", min);
  _csClient.write(buf);
}

void DCCExCS::setAckMax(uint16_t max) {
  char buf[18];
  sprintf(buf, "<D ACK MAX %d>", max);
  _csClient.write(buf);
}
