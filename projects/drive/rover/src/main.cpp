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
RFTransmitter RfTransmitter(PIN_PD5, 6);

u8 MAX_SPEED = 50;
u8 SPIN_SPEED = 50;
u8 ROTATE_SPEED = 50;
u8 MOVEMENT_SPEED = 100;
MotorDriverHwComponent MotorDriver({PIN_PB2, PIN_PB4, PIN_PB3}, {PIN_PB0, PIN_PD7, PIN_PB1}, 1000, 10, MAX_SPEED, SPIN_SPEED);

DistanceHwComponent DistanceSensor(PIN_PD6, PIN_PD2);

const u8 ComponentsLen = 8;
HwComponent* Components[ComponentsLen] = {
  &Laser,
  &LightSensor,
  &Display,
  &Switches,
  &RfReceiver,
  &RfTransmitter,
  &MotorDriver,
  &DistanceSensor,
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
  if(Display.shouldDraw() && MotorDriver.TargetSpeed == 0) {  
    Display.Scene = Switches[3].State ? Scenes::RxTxMsg : Switches[1].State ? Scenes::OpticsStats : Scenes::RfStats;
    // This takes a long time
    Display.draw();
  }
}

void motorUpdate() {
  MotorDriver.update();
}

void drive(Direction direction, u8 speed, time duration) {
  duration = map(duration, 0, 15, 10, 1000);
  MotorDriver.drive(direction, speed, duration);

  // ToDo: This should be "async", but other tasks are delaying it
  while(MotorDriver.TargetSpeed != 0) motorUpdate();
}

Instruction ack = {InstructionType::Ack, 0};
Instruction next;
u8 id = 0;
u8 last_id = 0;
void processInstruction() {
  if(last_id == id) return;
  last_id = id;

  switch (next.type)
  {
    case InstructionType::StopMovement:
      MotorDriver.drive(Direction::Stop, 0, 0);
      break;
    case InstructionType::RotateLeft:
      drive(Direction::Left, ROTATE_SPEED, next.data);
      break;
    case InstructionType::RotateRight:
      drive(Direction::Right, ROTATE_SPEED, next.data);
      break;
    case InstructionType::MoveBackward:
      drive(Direction::Backward, MOVEMENT_SPEED, next.data);
      break;
    case InstructionType::MoveForward:
      drive(Direction::Forward, MOVEMENT_SPEED, next.data);
      break;
    case InstructionType::ScanArea:
      // ToDo
      break;
  }
}

void laserReceive() {
  Laser.emit(LOW);
  if(!Switches[1].State)  return;

  time start = micros();
  u32 byte = 0;

  // Get instruction
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
    
    if(byte > 0 && next.decode(byte)) {
      memcpy(rxTx.receiveBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
      rxTx.received = MSG_BUFF_LEN;
      id++;
    }
  }

  // Send ack
  Laser.emit(LOW);
  delayMicroseconds(10);
  Laser.emit(HIGH);
  delayMicroseconds(10);
  Laser.emit(LOW);
  
  processInstruction();

  time end = micros();
  // 1 byte / duration = N bytes / 1 second
  if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
  
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

  if(lenght == 8 && next.decode(data)) {
    // Update display data
    memcpy(rxTx.receiveBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
    rxTx.received = MSG_BUFF_LEN;
    id++;
      
    // Send confirmation
    ack.data = id;
    lenght = ack.encode(data);
    RfTransmitter.transmit(data, lenght);
    RfReceiver.reset();
    
    processInstruction();
    
    // 1 byte / duration = N bytes / 1 second
    time ended = micros();
    if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  }
}


void distanceMeasure() {
  if(!Switches[2].State) return;

  // 1D
  double distance = 0, m = 0;
  double distance_sum = 0;
  time duration = 0;
  time duration_sum = 0;

  // Do multiple measurements
  for(u8 i = 0; i < 8; i++) {
    if(DistanceSensor.measure(distance, duration)) {
      distance_sum += distance;
      duration_sum += duration;
      m++;
    }
    delayMicroseconds(30);
  }
  if(m > 0) {
    distance_sum /= m;
    duration_sum /= m;
  }
  sprintf((char*)rxTx.transmitBuff, "%d mm", (int)distance_sum);
  rxTx.transmitted = MSG_BUFF_LEN;
}

Task tasks[] = {
  {1'000, &motorUpdate},
  {5'000, &rfReceive},
  {10'000, &distanceMeasure},
  {10'000, &laserReceive},
  {10'000, &displayUpdate},
};
TaskQueue scheduler(5, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}