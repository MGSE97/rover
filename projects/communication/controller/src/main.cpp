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

  time start = micros();
  u32 byte = 0;

  // Sync
  /*time synced;
  u32 ok = 0;
  u16 value = LightSensor.receive();
  for(u32 i = 0; i < 16; i++) {
    Laser.emit(value);
    delayMicroseconds(10);
    value = LightSensor.receive() > 512;
    if(value == Laser.Emitting && value == 0) {
      break;
      synced = micros();
    }
    else if(value != Laser.Emitting) ok++;
  }
  Laser.emit(LOW);

  Teleplot.sendUInt("ok", ok);
  if(ok < 4 || synced <= 0) return;

  while(micros() - synced < 20'000);*/

  u8 len = 8;
  // Send whole message
  for(u8 j = 0; j < MSG_BUFF_LEN; j++) {
    //u32 msg = encode_data(MESSAGE[j], 8, &len);
    u32 msg = MESSAGE[j];
    // Encoding needs N bits per byte
    for(u8 i = 0; i < len; i++) {
      Laser.emit(!((msg >> i) & 1) ? HIGH : LOW);
      delayMicroseconds(10);
    }
    
    rxTx.transmitBuff[j] = isPrintable(MESSAGE[j]) ? MESSAGE[j] : '#';
    rxTx.transmitted = j;
    if(msg == '\x03') break;
  }

  // Validate whole message
  for(u8 j = 0; j < MSG_BUFF_LEN; j++) {
    // Encoding needs N bits per byte
    for(u8 i = 0; i < len; i++) {
      delayMicroseconds(10);
      u16 value = LightSensor.receive();
      byte |= (value < 512) << (len - i - 1);
    }

    Teleplot.sendUInt("raw", byte);
    //byte = decode_data(byte, len);
    //Teleplot.sendUInt("hm", byte);
    
    if(byte > 0 && isPrintable(byte)) {
      rxTx.receiveBuff[j] = byte;
      rxTx.received = j;
    }
    if(byte == '\x03') break;
  }

  time end = micros();
  // 1 byte / duration = N bytes / 1 second
  if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
  
  Laser.emit(LOW);
  
  delayMicroseconds(10'000);
  displayUpdate();
}

void rfTransmit() {
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
  } while((ended - started) < 100'000);

  // Update display
  if(ack == 0) {
    rxTx.receiveBuff[rxTx.received] = '_';
    if(encoded == '\x03' || ++rxTx.received > MSG_BUFF_LEN) rxTx.received = 0;
  } 
  delayMicroseconds(5000);
  if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  displayUpdate();
}

Task tasks[] = {
  {100'000, &laserTransmit},
  {100'000, &rfTransmit},
  //{10'000, &displayUpdate},
};
TaskQueue scheduler(3, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}