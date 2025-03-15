#include "Types.h"

struct PinDefinition {
  pin Pin;
  volatile u8* OutputReg;
  volatile u8* InputReg;
  u8 BitMask;
};

PinDefinition getPinDefinition(pin pin);

/**
 * This is faster than digitalWrite function, but less safe.
 * It will skip timer checks.
 */ 
void fastDigitalWrite(PinDefinition& pin, u8 value);

/**
 * This is faster than digitalRead function, but less safe.
 * It will skip timer checks.
 */ 
u8 fastDigitalRead(PinDefinition& pin);