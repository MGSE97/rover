#include "info_display.h"

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

void laserReceive() {
  Laser.emit(LOW);
  if(!Switches[1].State)  return;

  time start = micros();
  u32 byte = 0;

  // Sync
  /*u32 ok = 0;
  for(u32 i = 0; i < 16; i++) {
    Laser.emitToggle();
    delayMicroseconds(10);
    u16 value = LightSensor.receive() > 512;
    if(value == Laser.Emitting) ok++;
  }
  Laser.emit(LOW);

  Teleplot.sendUInt("ok", ok);
  if(ok < 4) return;

  delayMicroseconds(20'000);*/

  // Get whole message
  u8 len = 8;
  for(u8 j = 0; j < MSG_BUFF_LEN; j++) {
    // Encoding needs N bits per byte
    for(u8 i = 0; i < len; i++) {
      delayMicroseconds(10);
      u16 value = LightSensor.receive();
      Teleplot.sendUInt("LS", value);
      byte |= (value < 512) << (len - 1 - i);
    }
    Teleplot.sendUInt("raw", byte);
    /*byte = decode_data(byte, 14);
    Teleplot.sendUInt("hm", byte);*/
    
    if(byte > 0 && isPrintable(byte)) {
      rxTx.receiveBuff[j] =  byte;
      rxTx.received = j + 1;
    }
    if(byte == '\x03') break;
  }

  // Send whole message
  for(u8 j = 0; j < MSG_BUFF_LEN; j++) {
    // Encoding needs N bits per byte
    //u8 msg = encode_data(rxTx.receiveBuff[j], 8, &len);
    u8 msg = rxTx.receiveBuff[j];
    for(u8 i = 0; i < len; i++) {
      Laser.emit(!((msg >> i) & 1) ? HIGH : LOW);
      delayMicroseconds(10);
    }
    
    rxTx.transmitBuff[j] = msg;
    rxTx.transmitted = j;
    if(msg == '\x03') break;
  }

  time end = micros();
  // 1 byte / duration = N bytes / 1 second
  if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
  
  Laser.emit(LOW);
}

void rfReceive() {
  if(Switches[2].State != RfReceiver.Enabled) {
    Switches[2].State ? RfReceiver.enable() : RfReceiver.disable();
    // We skip recieve, since component needs some time to receive datata
    return;
  }

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

Task tasks[] = {
  {10'000, &laserReceive},
  {10'000, &rfReceive},
  {10'000, &displayUpdate}
};
TaskQueue scheduler(3, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}