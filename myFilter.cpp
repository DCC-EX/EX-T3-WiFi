#include <Arduino.h>
#include <CommandDistributor.h>

void myFilter(Print *stream, byte &opcode, byte &paramCount, int16_t p[]) {
  (void)stream; // Stops warning
  if (opcode == 'U') {
    if (paramCount == 1 && p[0] == 'P') {
      CommandDistributor::broadcastPower();
    }
    opcode = '\0';
  }
};
