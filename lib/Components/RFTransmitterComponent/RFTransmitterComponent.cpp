#include "RFTransmitterComponent.h"

RFTransmitter::RFTransmitter(pin data, u8 protocol) {
  Data = data;
  Transmitter = RCSwitch();
  Transmitter.setProtocol(protocol);
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