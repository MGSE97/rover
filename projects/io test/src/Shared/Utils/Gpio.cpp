#include "Gpio.h"

PinDefinition getPinDefinition(pin pin) {
  return {
    pin,
    portOutputRegister(digitalPinToPort(pin)),
    portInputRegister(digitalPinToPort(pin)),
    digitalPinToBitMask(pin)
  };
}

void fastDigitalWrite(PinDefinition& pin, u8 value) {
  if(value) *pin.OutputReg |= pin.BitMask;
  else      *pin.OutputReg &= ~pin.BitMask;
}


u8 fastDigitalRead(PinDefinition& pin) {
  return (*pin.InputReg & pin.BitMask) ? HIGH : LOW;
}