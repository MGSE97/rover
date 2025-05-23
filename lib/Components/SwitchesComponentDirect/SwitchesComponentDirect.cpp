#include "SwitchesComponentDirect.h"

SwitchesHwComponentDirect::SwitchesHwComponentDirect(pin pins[6]) {
  for(u8 i; i < 6; i++) {
    Items[i] = {
      getPinDefinition(pins[i]),
      false
    };
  }
}

void SwitchesHwComponentDirect::init() {
  for(u8 i; i < 6; i++) {
    pinMode(Items[i].Pin.Pin, INPUT_PULLUP);
  }
  update();
}

void SwitchesHwComponentDirect::update() {
  for(u8 i; i < 6; i++) {
    bool value = fastDigitalRead(Items[i].Pin) == LOW;
    Changed = Changed || value != Items[i].State;
    Items[i].State = value;
  }
}