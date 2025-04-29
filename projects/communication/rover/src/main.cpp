#include "info_display.h"

const char* MESSAGE = "Mars Rover";

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

LaserHwComponent Laser(PIN_A0);

LightSensorHwComponent LightSensor(PIN_A6);

pin SwitchesPins[6] = {PIN_PB5, PIN_A1, PIN_A2, PIN_A3, PIN_A3, PIN_A3};
SwitchesHwComponentDirect Switches(SwitchesPins);

RFReceiver RfReceiver(PIN_PD4, PIN_PD3, 1);
RFTransmitter RfTransmitter(PIN_PD5, 1);

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
    Display.Scene = Switches[3].State ? Scenes::RxTxMsg : Switches[1].State ? Scenes::OpticsStats : Scenes::RfStats;
    Display.draw();
  }
}

void laserTransmit() {
  Laser.emit(LOW);
  if(!Switches[1].State)  return;

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
    rxTx.receiveBuff[rxTx.received] = isPrintable(byte) ? byte : '#';
    if(++rxTx.received > MSG_BUFF_LEN) rxTx.received = 0;
  }
}

void rfReceiveBasic() {
  /*if(Switches[2].State != RfReceiver.Enabled) {
    Switches[2].State ? RfReceiver.enable() : RfReceiver.disable();
    // We skip recieve, since component needs some time to receive datata
    return;
  }*/

  u8 lenght = 0;
  u32 data = 0;
  
  // Receive data
  time started = micros();
  lenght = RfReceiver.receive(data);

  Teleplot.sendUInt("In-D", lenght);
  Teleplot.sendUInt("In-V", data);
  if(lenght == 8 && data > 0) {
    // Update display data
    rxTx.receiveBuff[rxTx.received] = isPrintable(data) ? data : '#';
    rxTx.transmitBuff[rxTx.received] = rxTx.receiveBuff[rxTx.received];
    if(data == '\x03' || ++rxTx.received > MSG_BUFF_LEN) rxTx.received = 0;
    rxTx.transmitted = rxTx.received;
      
    // Send confirmation
    RfTransmitter.transmit(data, lenght);
    Teleplot.sendUInt("Out-D", lenght);
    Teleplot.sendUInt("Out-V", data);
    RfReceiver.reset();
    
    
    // 1 byte / duration = N bytes / 1 second
    time ended = micros();
    if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  }
  Serial.flush();
}

void rfReceiveBetter() {
  /*if(Switches[2].State != RfReceiver.Enabled) {
    Switches[2].State ? RfReceiver.enable() : RfReceiver.disable();
    // We skip recieve, since component needs some time to receive datata
    return;
  }*/

  u8 lenght = 0;
  u32 data = 0;
  Message received;
  
  // Receive data
  time started = micros();
  lenght = RfReceiver.receive(data);

  Teleplot.sendUInt("In-D", lenght);
  Teleplot.sendUInt("In-V", data);
  received.decode(data);
  
  Teleplot.sendUInt("In-S", received.sender);
  Teleplot.sendUInt("In-O", received.order);
  Teleplot.sendUInt("In-L", received.letter);
  if(lenght > 0 && received.decode(data) && received.sender == Device::Controller) {
    // Update display data
    rxTx.receiveBuff[received.order] = isPrintable(received.letter) ? received.letter : '#';
    rxTx.transmitBuff[received.order] = rxTx.receiveBuff[received.order];
      
    // Send confirmation
    Message ack = {
      Device::Rover,
      received.order,
      received.letter
    };
    u8 len = ack.encode(data);
    RfTransmitter.transmit(data, len);
    Teleplot.sendUInt("Out-V", data);
    Teleplot.sendUInt("Out-S", ack.sender);
    Teleplot.sendUInt("Out-O", ack.order);
    Teleplot.sendUInt("Out-L", ack.letter);
    RfReceiver.reset();
    
    
    // 1 byte / duration = N bytes / 1 second
    time ended = micros();
    if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  }
  Serial.flush();
}

Task tasks[] = {
  {100'000, &laserReceive},
  {100'000, &laserTransmit},
  {10'000, &rfReceiveBasic},
  //{10'000, &rfReceiveBetter},
  {10'000, &displayUpdate}
};
TaskQueue scheduler(4, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}