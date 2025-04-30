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

MotorDriverHwComponent MotorDriver({PIN_PB2, PIN_PB4, PIN_PB3}, {PIN_PB0, PIN_PD7, PIN_PB1}, 1000, 10, 50);

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
  if(Display.shouldDraw()) {  
    Display.Scene = Switches[3].State ? Scenes::RxTxMsg : Switches[1].State ? Scenes::OpticsStats : Scenes::RfStats;
    Display.draw();
  }
}

Instruction next;
u8 id = 0;
u8 last_id = 0;
void processInstruction() {
  if(last_id == id) return;
  last_id = id;

  switch (next.type)
  {
    case InstructionType::StopMovement:
      MotorDriver.drive(Direction::Stop, 0);
      break;
    case InstructionType::RotateLeft:
      MotorDriver.drive(Direction::Left, 50);
      delayMicroseconds(next.data);
      MotorDriver.drive(Direction::Stop, 0);
      break;
    case InstructionType::RotateRight:
      MotorDriver.drive(Direction::Right, 50);
      delayMicroseconds(next.data);
      MotorDriver.drive(Direction::Stop, 0);
      break;
    case InstructionType::MoveBackward:
      MotorDriver.drive(Direction::Backward, 50);
      delayMicroseconds(next.data);
      MotorDriver.drive(Direction::Stop, 0);
      break;
    case InstructionType::MoveForward:
      MotorDriver.drive(Direction::Forward, 50);
      delayMicroseconds(next.data);
      MotorDriver.drive(Direction::Stop, 0);
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
      rxTx.received = sizeof(INSTRUCTION_STR[next.type]);
      id++;
    }
  }

  // Send ack
  Laser.emit(LOW);
  delayMicroseconds(10);
  Laser.emit(HIGH);
  delayMicroseconds(10);
  
  processInstruction();

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
  if(lenght == 8 && next.decode(data)) {
    Serial.printf("%d %d %d\n", id, next.type, next.data);
    // Update display data
    memcpy(rxTx.receiveBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
    rxTx.received = sizeof(INSTRUCTION_STR[next.type]);
    id++;
      
    // Send confirmation
    RfTransmitter.transmit(1, 1);
    Teleplot.sendUInt("Out-D", lenght);
    Teleplot.sendUInt("Out-V", data);
    RfReceiver.reset();
    
    processInstruction();
    
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