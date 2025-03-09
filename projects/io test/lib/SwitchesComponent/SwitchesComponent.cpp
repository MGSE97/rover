#include "SwitchesComponent.h"

SwitchesHwComponent::SwitchesHwComponent(uint8_t ports[6]) {
  for(uint8_t i; i < 6; i++) {
    Items[i] = {
      ports[i],
      false
    };
  }
}

void SwitchesHwComponent::init() {
  for(uint8_t i; i < 6; i++) {
    pinMode(Items[i].Pin, INPUT_PULLUP);
  }
  update();
}

void SwitchesHwComponent::update() {
  for(uint8_t i; i < 6; i++) {
    bool value = digitalRead(Items[i].Pin) == LOW;
    Changed = Changed || value != Items[i].State;
    Items[i].State = value;
  }
}