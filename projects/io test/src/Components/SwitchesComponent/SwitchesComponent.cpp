#include "SwitchesComponent.h"

SwitchesHwComponent::SwitchesHwComponent(pin read, pin clk, pin reset) {
  Read = getPinDefinition(read);
  Clk = getPinDefinition(clk);
  Reset = getPinDefinition(reset);
  for(u8 i; i < 6; i++) {
    States[i] = false;
  }
}

void SwitchesHwComponent::init() {
  pinMode(Read.Pin, INPUT_PULLUP);
  pinMode(Clk.Pin, OUTPUT);
  pinMode(Reset.Pin, OUTPUT);
  fastDigitalWrite(Reset, HIGH);
  delay(10);
  update();
}

void SwitchesHwComponent::update() {
  // Reset
  fastDigitalWrite(Reset, LOW);
  delay(10);

  u8 value = 0;
  for(u8 i; i < 6; i++) {
    // Step to next
    fastDigitalWrite(Clk, HIGH);
    delay(10);
    fastDigitalWrite(Clk, LOW);
    delay(10);

    // Read value
    // ToDo: Current backflow through disabled pins
    Teleplot.sendInt("Switches", digitalRead(Read.Pin));
    
    delay(10);
  }
  
  fastDigitalWrite(Reset, HIGH);
}