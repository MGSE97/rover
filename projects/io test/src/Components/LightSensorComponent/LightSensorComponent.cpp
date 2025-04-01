#include "LightSensorComponent.h"

LightSensorHwComponent::LightSensorHwComponent(pin pin) {
  Pin = pin;
}

void LightSensorHwComponent::init() {
  pinMode(Pin, INPUT);
  enableFastAdc();
}

u16 LightSensorHwComponent::receive() {
  return analogRead(Pin);
}
