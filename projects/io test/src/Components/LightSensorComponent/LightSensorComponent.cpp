#include "LightSensorComponent.h"

LightSensorHwComponent::LightSensorHwComponent(pin pin) {
  Pin = pin;
}

LightSensorHwComponent::~LightSensorHwComponent() {
  digitalWrite(Pin, LOW);
}

void LightSensorHwComponent::init() {
  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);
  pinMode(Pin, INPUT);

  // Set ADC prescale to 16
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
}

u16 LightSensorHwComponent::receive() {
  return analogRead(Pin);
}
