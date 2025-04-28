/* Transmitter */

#include "info_display.h"


InfoDisplay Display(10, U8G_I2C_OPT_FAST);
time lastSceneChange = 0;

LaserHwComponent Laser(PIN_PD5);
time emitChange = 0;

LightSensorHwComponent LightSensor(PIN_A7);
LightSensorsData lightSensorsData={0,0};


pin SwitchesPins[6] = {PIN_PD7, PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4};
SwitchesHwComponentDirect Switches(SwitchesPins);
SwitchesData switchData = {{false,false,false,false,false,false}};

RFReceiver RfReceiver(PIN_PD3, PIN_PD2);
RFTransmitter RfTransmitter(PIN_PD4);
RFData RfData = {
  "RF Test\x03",
  "",
  7,
  0,
  0
};
const u8 charLenght = 8;
bool rfChanged = false;

const u8 ComponentsLen = 5;
HwComponent* Components[ComponentsLen] = {
  &Laser,
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

  Display.LightSensors = &lightSensorsData;
  Display.Switch = &switchData;
  Display.RfData = &RfData;

  Serial.flush();
}

time now;

void laserTransmit() {
  if(Switches[0].State) {
    Laser.emitToggle();
  } else {
    Laser.emit(LOW);
  }
}

void laserReceive() {
  lightSensorsData.value1 = LightSensor.receive();

  Teleplot.sendInt("LS1", lightSensorsData.value1);
  Teleplot.sendInt("LS2", lightSensorsData.value2);
}

void rfTransmit() {
  if(!Switches[1].State) return;

  Teleplot.sendUInt("RfTx", RfData.transmitted);
  RfTransmitter.transmit(RfData.transmitBuff[RfData.transmitted], charLenght);
  RfData.transmitted++;
  if(RfData.transmitted > RfData.transmitLenght) RfData.transmitted = 0;
  rfChanged = true;
}

void rfReceive() {
  if(Switches[1].State != RfReceiver.Enabled) {
    Switches[1].State ? RfReceiver.enable() : RfReceiver.disable();
    // We skip recieve, since component needs some time to receive datata
    return;
  }

  u8 lenght = RfReceiver.receive(RfData.receiveBuff[RfData.received]);
  if(lenght > 0) {
    Teleplot.sendUInt("RfRx", RfData.received);
    if(RfData.receiveBuff[RfData.received] == '\x03' || RfData.received >= 9) RfData.received = 0;
    else RfData.received++;
    rfChanged = true;
  }
}

void displayUpdate() {
  bool canSceneChange = now - lastSceneChange > 2'000'000;
  Teleplot.sendInt("LSC", now - lastSceneChange);
  Teleplot.sendInt("CSC", canSceneChange);

  if(canSceneChange) {
    if(Switches.Changed) {
      Display.Scene = SwitchesStatus;
      Switches.Changed = false;
      lastSceneChange = now;
    }
    else if(lightSensorsData.value1 > 500 || lightSensorsData.value2 > 500) {
      Display.Scene = LightSensorStatus;
      lastSceneChange = now;
    }
    else if(rfChanged) {
      Display.Scene = RFStatus;
      rfChanged = false;
    }
    else {
      Display.Scene = PoweredOnStats;
    }
  }
  
  switch (Display.Scene)
  {
    case SwitchesStatus:
      for(u8 i = 0; i < 6; i++){
        switchData.values[i] = Switches[i].State;
      }
      break;
    default:
      break;
  }

  Display.draw();
}

struct Task {
  time lastRun;
  time delay;
  void (*run)();
};

Task tasks[] = {
  {0, 50'000, &laserReceive},
  {0, 500'000, &laserTransmit},
  {0, 10'000, &displayUpdate},
  {0, 5'000, &rfReceive},
  {0, 100'000, &rfTransmit},
};

u8 task_len = 5;
Task* task;

void loop() {
  now = micros();
  Switches.update();

  // Run active tasks
  for(u8 i = 0; i < task_len; i++) {
    task = &tasks[i];
    if(now - task->lastRun >= task->delay) {
      task->run();
      task->lastRun = now;
    }
  }

  Serial.flush();
}