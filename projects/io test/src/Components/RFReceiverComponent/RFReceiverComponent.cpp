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
  digitalWrite(Enable, HIGH);
  Receiver.enableReceive(digitalPinToInterrupt(Data));
}

void RFReceiver::disable() {
  Enabled = false;
  digitalWrite(Enable, LOW);
  Receiver.disableReceive();
}

u8 RFReceiver::receive(u8& value) {
  if(!Receiver.available()) return 0;

  u8 length = Receiver.getReceivedBitlength();
  value = Receiver.getReceivedValue();

  Receiver.resetAvailable();
  return length;
}
