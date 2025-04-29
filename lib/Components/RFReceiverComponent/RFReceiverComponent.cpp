#include "RFReceiverComponent.h"

RFReceiver::RFReceiver(pin enable, pin data, u8 protocol) {
  Data = data;
  Enable = enable;
  Receiver = RCSwitch();
  Protocol = protocol;
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

u8 RFReceiver::receive(u32& value) {
  if(!Receiver.available()) return 0;

  Teleplot.sendInt("Px", Receiver.getReceivedProtocol());
  Teleplot.sendInt("Pe", Protocol); 
  if(Receiver.getReceivedProtocol() == Protocol) {
    u8 length = Receiver.getReceivedBitlength();
    value = Receiver.getReceivedValue();
    Teleplot.sendInt("Len", length); 
    
    Receiver.resetAvailable();
    return length;
  }

  Receiver.resetAvailable();
  return 0;
}
