#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <charconv>

// https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c#comment32189462_2745074
// https://stackoverflow.com/a/14878734
inline uint8_t divideAndCeil(uint8_t i, uint8_t div) {
  return i / div + (i % div != 0);
}

template <typename T>
inline T fromChars(std::string str) {
  T result;
  std::from_chars(str.c_str(), str.c_str() + str.size(), result);
  return result;
};

inline void randomChars(char* buf, uint8_t len, const char* set = "abcdefghijklmnopqrstuvwxyz0123456789") {
  uint8_t setLen = strlen(set);

  for (uint8_t i = 0; i < len; i++) {
    uint8_t r = esp_random() % (setLen - 1);
    buf[i] = set[r];
  }
}

#endif
