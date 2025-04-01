#include "SwitchesComponent.h"

SwitchesHwComponent::SwitchesHwComponent(pin pins[6]) {
  for(u8 i; i < 6; i++) {
    Items[i] = {
      getPinDefinition(pins[i]),
      false
    };
  }
}

void SwitchesHwComponent::init() {
  for(u8 i; i < 6; i++) {
    pinMode(Items[i].Pin.Pin, INPUT_PULLUP);
  }
  update();
}

void SwitchesHwComponent::update() {
  for(u8 i; i < 6; i++) {
    bool value = fastDigitalRead(Items[i].Pin) == LOW;
    Changed = Changed || value != Items[i].State;
    Items[i].State = value;
  }
}