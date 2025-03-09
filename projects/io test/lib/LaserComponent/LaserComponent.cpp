#pragma once
#include "LaserComponent.h"

LaserHwComponent::LaserHwComponent(uint8_t pin) {
  Pin = pin;
  Reg = portOutputRegister(digitalPinToPort(pin));
  BitMask = digitalPinToBitMask(pin);
  Emitting = false;
}

LaserHwComponent::~LaserHwComponent() {
  digitalWrite(Pin, LOW);
}

void LaserHwComponent::init() {
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);
}

void LaserHwComponent::emit(uint8_t value) {
  Emitting = value;
  // This is faster than digitalWrite function, but less safe
  if(value) *Reg |= BitMask;
  else      *Reg &= ~BitMask;
}

void LaserHwComponent::emitToggle() {
  emit(!Emitting);
}
