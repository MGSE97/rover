#include "RFReceiverComponent.h"

RFReceiver::RFReceiver(pin enable, pin data) {
  Data = data;
  Enable = enable;
  Receiver = RCSwitch();
}

RFReceiver::~RFReceiver() {}

void RFReceiver::init() {
  pinMode(Enable, OUTPUT);
  pinMode(Data, INPUT);
  
  enable();
}

void RFReceiver::enable() {
  Enabled = true;
  Receiver.enableReceive(digitalPinToInterrupt(Data));
  digitalWrite(Enable, HIGH);
}

void RFReceiver::disable() {
  Enabled = false;
  Receiver.disableReceive();
  digitalWrite(Enable, LOW);
}

u8 RFReceiver::receive(u8& value) {
  if(!Receiver.available()) return 0;

  u8 length = Receiver.getReceivedBitlength();
  value = Receiver.getReceivedValue();

  Receiver.resetAvailable();
  return length;
}
