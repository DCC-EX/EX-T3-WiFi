#ifndef VERSION_H
#define VERSION_H

#include <Arduino.h>

constexpr uint32_t VERSION_TO_INT32(uint8_t major, uint8_t minor, uint8_t patch) {
  return major << 20 | minor << 10 | patch;
}

const uint8_t T3_VERSION_MAJOR = 0;
const uint8_t T3_VERSION_MINOR = 1;
const uint8_t T3_VERSION_PATCH = 1;
const uint32_t T3_VERSION = VERSION_TO_INT32(T3_VERSION_MAJOR, T3_VERSION_MINOR, T3_VERSION_PATCH);

#endif
