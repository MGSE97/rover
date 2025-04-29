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
  Receiver.resetAvailable();
}

void RFReceiver::disable() {
  Enabled = false;
  digitalWrite(Enable, LOW);
  Receiver.disableReceive();
}

void RFReceiver::reset() {
  Receiver.resetAvailable();
}

u8 RFReceiver::receive(u32& value) {
  // Clean up buffers
  if(!Receiver.available()) {
    Receiver.resetAvailable();
  }
  
  // Wait for message
  time started = micros();
  while (!Receiver.available() || (micros() - started) < 10'000) {
    delayMicroseconds(100);
  };
  
  // Check and return message
  Teleplot.sendUInt("Px", Receiver.getReceivedProtocol());
  Teleplot.sendUInt("Pe", Protocol); 
  if(Receiver.getReceivedProtocol() == Protocol) {
    u8 length = Receiver.getReceivedBitlength();
    value = Receiver.getReceivedValue();
    Teleplot.sendUInt("Len", length); 
    Teleplot.sendUInt("Val", value); 
    
    Receiver.resetAvailable();
    return length;
  }

  Receiver.resetAvailable();
  return 0;
}
