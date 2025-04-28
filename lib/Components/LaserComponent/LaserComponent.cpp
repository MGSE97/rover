#include "LaserComponent.h"

LaserHwComponent::LaserHwComponent(pin pin) {
  Pin = getPinDefinition(pin);
  Emitting = false;
}

LaserHwComponent::~LaserHwComponent() {
  digitalWrite(Pin.Pin, LOW);
}

void LaserHwComponent::init() {
  pinMode(Pin.Pin, OUTPUT);
  digitalWrite(Pin.Pin, LOW);
}

void LaserHwComponent::emit(u8 value) {
  Emitting = value;
  fastDigitalWrite(Pin, value);
}

void LaserHwComponent::emitToggle() {
  emit(!Emitting);
}
