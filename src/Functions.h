#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>

// https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c#comment32189462_2745074
// https://stackoverflow.com/a/14878734
inline uint8_t divideAndCeil(uint8_t i, uint8_t div) {
  return i / div + (i % div != 0);
}

#endif
