#include "info_display.h"


InfoDisplay Display(10, U8G_I2C_OPT_FAST);
time lastSceneChange = 0;

LaserHwComponent Laser(PIN_PB4);
time emitChange = 0;

LightSensorHwComponent LightSensor1(PIN_A6);
LightSensorHwComponent LightSensor2(PIN_A7);
LightSensorsData lightSensorsData={0,0};

MotorDriverHwComponent MotorDriver({PIN_PB3, PIN_PB7, PIN_PB5}, {PIN_PB1, PIN_PB0, PIN_PD6}, 100, 50);
MotorDriverData motorDriverData={Stop,50};
time driveChange = 0;
bool increaseSpeed = true;

pin SwitchesPins[6] = {PIN_A2, PIN_A2, PIN_A2, PIN_A2, PIN_A2, PIN_A3};
SwitchesHwComponent Switches(PIN_A1, PIN_A3, PIN_A2);
SwitchesData switchData = {{false,false,false,false,false,false}};

Distance2DHwComponent DistanceSensor(PIN_PD7, PIN_PD3, PIN_A0);
DistanceSensorsData distanceSensorsData={0,0};
bool distanceChanged = false;

RFReceiver RfReceiver(PIN_PD4, PIN_PD2);
RFTransmitter RfTransmitter(PIN_PB2);
RFData RfData = {
  "RF Test\x03",
  "",
  7,
  0,
  0
};
const u8 charLenght = 8;
bool rfChanged = false;

const u8 ComponentsLen = 9;
HwComponent* Components[ComponentsLen] = {
  &Laser,
  &Display,
  &LightSensor1,
  &LightSensor2,
  &MotorDriver,
  &Switches,
  &DistanceSensor,
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
  Display.MotorDriver = &motorDriverData;
  Display.Switch = &switchData;
  Display.DistanceSensor = &distanceSensorsData;
  Display.RfData = &RfData;

  Serial.flush();
}

time now;

void laserTransmit() {
  if(Switches[4]) {
    Laser.emitToggle();
  } else {
    Laser.emit(LOW);
  }
}

void laserReceive() {
  lightSensorsData.value1 = LightSensor1.receive();
  lightSensorsData.value2 = LightSensor2.receive();

  Teleplot.sendInt("LS1", lightSensorsData.value1);
  Teleplot.sendInt("LS2", lightSensorsData.value2);
}

void distanceMeasure() {
  //if(!Switches[3]) return;

  // 1D
  /*double distance = 0, m = 0;
  time duration = 0;
  
  distanceSensorsData.distance = 0;
  distanceSensorsData.duration = 0;

  // Do multiple measurements
  for(u8 i = 0; i < 32; i++) {
    if(DistanceSensor.measure(distance, duration)) {
      distanceSensorsData.distance += distance;
      distanceSensorsData.duration += duration;
      distanceChanged = true;
      m++;
    }
    delayMicroseconds(30);
  }
  if(m > 0) {
    distanceSensorsData.distance /= m;
    distanceSensorsData.duration /= m;
  }
  Teleplot.sendInt("Dur", distanceSensorsData.duration);
  Teleplot.sendDouble("Dst", distanceSensorsData.distance);
  Teleplot.sendInt("M", (int)m);*/

  // 2D
  Serial.printf(">sens:0:0;55:0|xy\n");

  Point obj[10];
  u8 i = 0;
  time started = micros();
  do {
    if(DistanceSensor.measure(obj[i])) i++;
  } while(i < 10 && micros() - started < 100'000);
  
  for(u8 j = 0; j < i; j++) {  
    if(j == 0) Serial.printf(">obj:%ld:%ld", obj[j].X, obj[j].Y);
    else Serial.printf(";%ld:%ld", obj[j].X, obj[j].Y);
  }
  Serial.printf("|xy\n");
}

void motorDrive() {
  if(Switches[5]) {
    if(motorDriverData.speed == 0) {
      switch(motorDriverData.direction) {
        case Stop:
          motorDriverData.direction = Left;
          break;
        case Left:
          motorDriverData.direction = Right;
          break;
        case Right:
          motorDriverData.direction = Stop;
          break;
        default:
          break;
      }
      increaseSpeed = true;
    }
    
    if(increaseSpeed) motorDriverData.speed+=2;
    else              motorDriverData.speed-=2;

    if(motorDriverData.speed > 100) increaseSpeed = false;
    
    MotorDriver.drive(motorDriverData.direction, motorDriverData.speed);
  } else {
    MotorDriver.drive(Stop, 0);
  }
}

void rfTransmit() {
  Teleplot.sendUInt("RfTx", RfData.transmitted);
  RfTransmitter.transmit(RfData.transmitBuff[RfData.transmitted], charLenght);
  RfData.transmitted++;
  if(RfData.transmitted > RfData.transmitLenght) RfData.transmitted = 0;
  rfChanged = true;
}

void rfReceive() {
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
    else if(motorDriverData.direction != Stop) {
      Display.Scene = MotorDriverStatus;
      lastSceneChange = now;
    }
    else if(distanceChanged) {
      Display.Scene = DistanceSensorStatus;
      distanceChanged = false;
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
        switchData.values[i] = Switches[i];
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
  {0, 10, &distanceMeasure},
  {0, 50'000, &laserReceive},
  {0, 500'000, &laserTransmit},
  {0, 20'000, &motorDrive},
  {0, 10'000, &displayUpdate},
  {0, 5'000, &rfReceive},
  {0, 100'000, &rfTransmit},
};

u8 task_len = 7;
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