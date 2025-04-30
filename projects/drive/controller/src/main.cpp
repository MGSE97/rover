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

Instruction next;
u8 id = 0;
u8 last_id = 0;
void processInstruction() {
  if(!Serial.available()) return;
  String payload = Serial.readStringUntil('\n');
  Serial.println(payload);
  u8 cmd; int data;
  sscanf(payload.c_str(), "%c%i", &cmd, &data);
  next.data = (u8)data;
  switch (cmd)
  {
    case 'x':
    case 'X':
      next.type = InstructionType::StopMovement;
      break;
    case 'a':
    case 'A':
      next.type = InstructionType::RotateLeft;
      break;
    case 'd':
    case 'D':
      next.type = InstructionType::RotateRight;
      break;
    case 'w':
    case 'W':
      next.type = InstructionType::MoveForward;
      break;
    case 's':
    case 'S':
      next.type = InstructionType::MoveBackward;
      break;
    case 'f':
    case 'F':
      next.type = InstructionType::ScanArea;
      break;
    default:
      return;
  };
  id++;
}


void laserTransmit() {
  Laser.emit(LOW);
  if(!Switches[0].State)  return;
  if(last_id == id) return;
  last_id = id;
  Serial.printf("Optics - %d. %s %d\n", id, INSTRUCTION_STR[next.type], next.data);

  time start = micros();
  u32 byte = 0;

  u32 msg;
  u8 len = next.encode(msg);

  // Send instruction
  // Encoding needs N bits per byte
  for(u8 i = 0; i < len; i++) {
    Laser.emit(!((msg >> i) & 1) ? HIGH : LOW);
    delayMicroseconds(10);
  }
  
  memcpy(rxTx.transmitBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
  rxTx.transmitted = MSG_BUFF_LEN;

  // Wait for ack
  delayMicroseconds(10);
  u16 value = LightSensor.receive();
  Serial.println(value < 512 ? "Ack" : "Failed");

  time end = micros();
  // 1 byte / duration = N bytes / 1 second
  if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
  
  Laser.emit(LOW);
}

void rfTransmit() {
  if(!Switches[1].State) return;
  if(last_id == id) return;
  last_id = id;
  Serial.printf("Rf - %d. %s %d\n", id, INSTRUCTION_STR[next.type], next.data);
  
  u32 encoded;
  u8 len = next.encode(encoded);

  // Update display
  memcpy(rxTx.transmitBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
  rxTx.transmitted = MSG_BUFF_LEN;
  displayUpdate();
  
  u32 ack = 0;
  
  time started = micros();
  time ended = 0;
  
  do {
    Serial.flush();
    // Send data
    Teleplot.sendUInt("Out", encoded);
    RfTransmitter.transmit(encoded, len);
    
    // Wait for confirmation
    u8 lenght = RfReceiver.receive(ack);
    ended = micros();

    Teleplot.sendUInt("In-L", lenght);
    if(lenght == 0) ack = 0;
    else {
      Serial.println(ack ? "Ack" : "Failed");
    }
  } while((ended - started) < 100'000);

  // Update display
  delayMicroseconds(5000);
  if(ended - started > 0) rf.topSpeed = 1e6 / (ended - started);
  displayUpdate();
}

Task tasks[] = {
  {100'000, &laserTransmit},
  {100'000, &rfTransmit},
  {10'000, &displayUpdate},
  {1'000, &processInstruction},
};
TaskQueue scheduler(4, tasks);

void loop() {
  Switches.update();

  // Run active tasks
  scheduler.process();

  Serial.flush();
}