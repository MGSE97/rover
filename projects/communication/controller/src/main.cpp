#include "info_display.h"

const char* MESSAGE = "Controller\x03";

InfoDisplay Display(10, U8G_I2C_OPT_FAST);
RxTxData rxTx = {
  "           ",
  "           ",
  0,
  0
};
StatsData optics = {
  Method::Optics,
  0
};
StatsData rf = {
  Method::RF,
  0
};

LaserHwComponent Laser(PIN_PD5);

LightSensorHwComponent LightSensor(PIN_A7);

pin SwitchesPins[6] = {PIN_PD7, PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4};
SwitchesHwComponentDirect Switches(SwitchesPins);

RFReceiver RfReceiver(PIN_PD3, PIN_PD2, 1);
RFTransmitter RfTransmitter(PIN_PD4, 1);

const u8 ComponentsLen = 6;
HwComponent* Components[ComponentsLen] = {
  &Laser,
  &LightSensor,
  &Display,
  &Switches,
  &RfReceiver,
  &RfTransmitter,
};

// put your setup code here, to run once
void setup() {
  // Serial monitor output stup
  Serial.begin(230400);
  Serial.println("Serial monitor connected.");
  
  for(u8 i = 0; i < ComponentsLen; i++) {
    Components[i]->init();
  }
  
  Display.RxTx = &rxTx;
  Display.Optics = &optics;
  Display.Rf = &rf;
  
  Serial.flush();
}

void displayUpdate() {
  if(Display.shouldDraw()) {
    Display.Scene = Switches[2].State ? Scenes::RxTxMsg : Switches[0].State ? Scenes::OpticsStats : Scenes::RfStats;
    Display.draw();
  }
}

void laserTransmit() {
  Laser.emit(LOW);
  if(!Switches[0].State)  return;

  u8 len = 0;
  u32 byte = encode_data(MESSAGE[rxTx.transmitted], 8, &len);
  for(u32 i = 0; i < len; i++) {
    Laser.emit((byte & (1 << i)) ? HIGH : LOW);
    delayMicroseconds(1000);
  }
  rxTx.transmitBuff[rxTx.transmitted] = MESSAGE[rxTx.transmitted];
  if(++rxTx.transmitted > MSG_BUFF_LEN) rxTx.transmitted = 0;
  
  Laser.emit(LOW);
}

void laserReceive() {
  time start = micros();
  u32 byte = 0;
  // Hamming encoding needs 14 bits per byte
  for(u32 i = 0; i < 14; i++) {
    u16 value = LightSensor.receive();
    delayMicroseconds(1000);
    Teleplot.sendInt("LS", value);
    byte |= (value > 512) << (13-i);
  }
  time end = micros();
  byte = decode_data(byte, 14);

  if(byte != 0) {
    // 1 byte / duration = N bytes / 1 second
    if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
    if(byte == '\x03') rxTx.received = 0;
    else {
      rxTx.receiveBuff[rxTx.received] = isPrintable(byte) ? byte : '#';
      if(++rxTx.received > MSG_BUFF_LEN) rxTx.received = 0;
    }
  }
}

void rfTransmitBasic() {
  if(!Switches[1].State) return;
  
  u8 encoded = MESSAGE[rxTx.transmitted];

  // Update display
  rxTx.transmitBuff[rxTx.transmitted] = MESSAGE[rxTx.transmitted];
  if(encoded == '\x03' || ++rxTx.transmitted > MSG_BUFF_LEN) rxTx.transmitted = 0;
  displayUpdate();
  
  u32 ack = 0;
  
  time started = micros();
  time ended = 0;
  
  do {
    Serial.flush();
    // Send data
    Teleplot.sendUInt("Out", encoded);
    RfTransmitter.transmit(encoded, 8);
    RfReceiver.reset();
    
    // Wait for confirmation
    u8 lenght = RfReceiver.receive(ack);
    ended = micros();

    Teleplot.sendUInt("In-L", lenght);
    if(lenght != 8) ack = 0;
    else {
      Teleplot.sendUInt("In-V", ack);
      // Validate received data, and stop loop if response is valid
      if(ack != encoded) ack = 0;
      else {      
        rxTx.receiveBuff[rxTx.received] = isPrintable(ack) ? ack : '#';
        if(encoded == '\x03' || ++rxTx.received > MSG_BUFF_LEN) rxTx.received = 0;
        break;
      }
    }
  } while((ended - started) < 50'000);

  // Update display
  if(ack == 0 && (encoded == '\x03' || ++rxTx.received > MSG_BUFF_LEN)) rxTx.received = 0;
  delayMicroseconds(5000);
  if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  displayUpdate();
}

void rfTransmitBetter() {
  if(!Switches[1].State) return;
  
  Message msg = {
    Device::Controller,
    rxTx.transmitted,
    (u8)MESSAGE[rxTx.transmitted],
  };
  Message decoded;

  // Update display
  rxTx.transmitBuff[rxTx.transmitted] = MESSAGE[rxTx.transmitted];
  if(++rxTx.transmitted > MSG_BUFF_LEN) rxTx.transmitted = 0;
  displayUpdate();
  
  u32 encoded = 0;
  u8 len = msg.encode(encoded);
  u32 ack = 0;
  
  time started = micros();
  time ended = 0;
  
  Teleplot.sendUInt("Out-D", len);
  Teleplot.sendUInt("Out-V", encoded);
  Teleplot.sendUInt("Out-S", msg.sender);
  Teleplot.sendUInt("Out-O", msg.order);
  Teleplot.sendUInt("Out-L", msg.letter);
  do {
    Serial.flush();
    // Send data
    Teleplot.sendUInt("Out", encoded);
    RfTransmitter.transmit(encoded, len);
    RfReceiver.reset();
    
    // Wait for confirmation
    u8 lenght = RfReceiver.receive(ack);
    ended = micros();

    if(lenght == 0) ack = 0;
    else {
      Teleplot.sendUInt("In-V", ack);
      Teleplot.sendUInt("In-S", decoded.sender);
      Teleplot.sendUInt("In-O", decoded.order);
      Teleplot.sendUInt("In-L", decoded.letter);
      // Validate received data, and stop loop if response is valid
      if(!decoded.decode(ack) || decoded.sender != Device::Rover) ack = 0;
      else if(decoded.letter == msg.letter && decoded.order == msg.order) break;
    }
  } while(ended - started < 50'000);

  // Update display
  delayMicroseconds(5000);
  if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  displayUpdate();
}

Task tasks[] = {
  {100'000, &laserReceive},
  {100'000, &laserTransmit},
  {100'000, &rfTransmitBasic},
  //{100'000, &rfTransmitBetter},
  //{10'000, &displayUpdate},
};
TaskQueue scheduler(3, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}