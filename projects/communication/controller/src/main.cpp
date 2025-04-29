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

RFReceiver RfReceiver(PIN_PD3, PIN_PD2, 3);
RFTransmitter RfTransmitter(PIN_PD4, 2);

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
  if(rxTx.transmitted++ > MSG_BUFF_LEN) rxTx.transmitted = 0;
  
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
      if(rxTx.received++ > MSG_BUFF_LEN) rxTx.received = 0;
    }
  }
}

void rfTransmit() {
  if(!Switches[1].State) return;

  /*u8 len = 0;
  u32 byte = encode_data(MESSAGE[rxTx.transmitted], 8, &len);
  RfTransmitter.transmit(byte, len);*/
  Message msg = {
    Device::Controller,
    rxTx.transmitted,
    (u8)MESSAGE[rxTx.transmitted],
  };
  Message decoded;
  u32 encoded = 0;
  u8 len = msg.encode(encoded);
  u32 ack = 0;
  for(u8 i = 0; i < 10; i++) {
    // Send data
    RfTransmitter.transmit(encoded, len);
    
    // Wait for confirmation
    delayMicroseconds(5000);
    u8 lenght = RfReceiver.receive(ack);
    if(lenght == 0) ack = 0;
    else {
      if(!decoded.decode(ack) || decoded.sender != Device::Rover) ack = 0;
      else if(decoded.letter == msg.letter && decoded.order == msg.order) break;
    }
  };
  
  rxTx.transmitBuff[rxTx.transmitted] = MESSAGE[rxTx.transmitted];
  if(rxTx.transmitted++ > MSG_BUFF_LEN) rxTx.transmitted = 0;
}

void rfReceive() {
  /*if(Switches[1].State != RfReceiver.Enabled) {
    Switches[1].State ? RfReceiver.enable() : RfReceiver.disable();
    // We skip recieve, since component needs some time to receive datata
    return;
  }*/

  time start = micros();
  u8 lenght = 0;
  u32 data = 0;
  Message received;
  for(u8 i = 0; i < 10; i++) {
    // Get data
    lenght = RfReceiver.receive(data);
    time end = micros();
    if(lenght > 0 && received.decode(data) && received.sender == Device::Rover) {
      //byte = decode_data(byte, lenght);

      // 1 byte / duration = N bytes / 1 second
      if(end - start > 0) rf.topSpeed = 1e6 / (end - start);
      if(received.letter == '\x03') rxTx.received = 0;
      else {
        rxTx.receiveBuff[received.order] = isPrintable(received.letter) ? received.letter : '#';
      }
      
      // Send confirmation
      Message ack = {
        Device::Controller,
        received.order,
        received.letter
      };
      u8 len = ack.encode(data);
      RfTransmitter.transmit(data, len);
      break;
    }
    delayMicroseconds(5000);
  };
}

void displayUpdate() {
  if(Display.shouldDraw()) {
    Display.Scene = Switches[2].State ? Scenes::RxTxMsg : Switches[0].State ? Scenes::OpticsStats : Scenes::RfStats;
    Display.draw();
  }
}

Task tasks[] = {
  {100'000, &laserReceive},
  {100'000, &laserTransmit},
  {5'000, &rfReceive},
  {100'000, &rfTransmit},
  {10'000, &displayUpdate},
};
TaskQueue scheduler(5, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}