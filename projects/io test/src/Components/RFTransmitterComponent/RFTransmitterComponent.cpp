#include "RFTransmitterComponent.h"

RFTransmitter::RFTransmitter(pin data) {
  Data = data;
  Transmitter = RCSwitch();
}

RFTransmitter::~RFTransmitter() {}

void RFTransmitter::init() {
  pinMode(Data, INPUT);
  digitalWrite(Data, LOW);

  Transmitter.enableTransmit(Data);
}

void RFTransmitter::transmit(u8 value, u8 lengh) {
  Transmitter.send(value, lengh);
}