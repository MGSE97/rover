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

RFReceiver RfReceiver(PIN_PD3, PIN_PD2, 6);
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

void drawUI(bool has_ack);

// put your setup code here, to run once
void setup() {
  // Serial monitor output stup
  Serial.begin(230400);
  Serial.println("Serial monitor connected.");
  Serial.setTimeout(10);
  
  for(u8 i = 0; i < ComponentsLen; i++) {
    Components[i]->init();
  }
  
  Display.RxTx = &rxTx;
  Display.Optics = &optics;
  Display.Rf = &rf;
  
  drawUI(false);
  Serial.flush();
}

void displayUpdate() {
  if(Display.shouldDraw()) {
    Display.Scene = Switches[2].State ? Scenes::RxTxMsg : Switches[0].State ? Scenes::OpticsStats : Scenes::RfStats;
    Display.draw();
  }
}

Instruction ack;
Instruction next;
u8 last_char = 0;
u8 id = 0;
u8 last_id = 0;
u8 speedFB = 5;
u8 speedLR = 3;
bool ui_drawn = false;

const char* OK = "\033[32;40m";
const char* NEXT = "\033[35;40m";
const char* ERR = "\033[31;40m";
const char* OFF = "\033[37;40m";
const char* DEF = "\033[0m";
const char* ENDL = "                \n";
const char* OFFSET = "   ";

const char* getState(InstructionType type, bool has_ack) {
  if(next.type != type) return OFF;
  if(!has_ack) return NEXT;
  if(ack.type == InstructionType::Ack) return OK;
  return ERR;
}

struct Btn {
  const char* action;
  const char letter;
};

const Btn BUTTONS[10] = {
  {"-", 'Q'},
  {"↑", 'W'},
  {"+", 'E'},
  {"+", 'R'},
  
  {"←", 'A'},
  {"↓", 'S'},
  {"→", 'D'},
  {"-", 'F'},
  
  {"■", 'X'},
  {"@", 'C'},
};

void btn(u8 id, u8 step, const char* state) {
  if(step == 0)      Serial.printf("%s╔═══╗", state);
  else if(step == 1) Serial.printf("%s║ %s ║", state, BUTTONS[id].action);
  else if(step == 2) Serial.printf("%s║ %c ║", state, BUTTONS[id].letter);
  else               Serial.printf("%s╚═══╝", state);
}

void drawUI(bool has_ack) {
  // Compute buttons state
  const char* w = getState(InstructionType::MoveForward, has_ack);
  const char* s = getState(InstructionType::MoveBackward, has_ack);
  const char* a = getState(InstructionType::RotateLeft, has_ack);
  const char* d = getState(InstructionType::RotateRight, has_ack);
  const char* x = getState(InstructionType::StopMovement, has_ack);
  const char* c = getState(InstructionType::ScanArea, has_ack);
  const char* q = last_char == 'q' ? OK : OFF;
  const char* e = last_char == 'e' ? OK : OFF;
  const char* r = last_char == 'r' ? OK : OFF;
  const char* f = last_char == 'f' ? OK : OFF;
  const char* method = Switches[0].State ? "Optics":  Switches[1].State ? "RF" : "None";

  if(ui_drawn) {
    for(u8 n = 0; n < 12; n++) {
      // Move cursor up
      Serial.print("\033[F");
    }
  }

  // Draw buttons
  btn(0,0,q); btn(1,0,w); btn(2,0,e); btn(3,0,r); Serial.printf("%s Instruction%s",DEF,ENDL);
  btn(0,1,q); btn(1,1,w); btn(2,1,e); btn(3,1,r); Serial.printf("%s   Id:    %d%s",DEF,id,ENDL);
  btn(0,2,q); btn(1,2,w); btn(2,2,e); btn(3,2,r); Serial.printf("%s   Type:  %s%s",DEF,INSTRUCTION_STR[next.type],ENDL);
  btn(0,3,q); btn(1,3,w); btn(2,3,e); btn(3,3,r); Serial.printf("%s   Speed: %d%s",DEF,next.data,ENDL);
  
  btn(4,0,a); btn(5,0,s); btn(6,0,d); btn(7,0,f); Serial.printf("%s            %s",DEF,ENDL);
  btn(4,1,a); btn(5,1,s); btn(6,1,d); btn(7,1,f); Serial.printf("%s Ack        %s",DEF,ENDL);
  btn(4,2,a); btn(5,2,s); btn(6,2,d); btn(7,2,f); Serial.printf("%s   Id:    %d%s",DEF,ack.data,ENDL);
  btn(4,3,a); btn(5,3,s); btn(6,3,d); btn(7,3,f); Serial.printf("%s   Type:  %s%s",DEF,INSTRUCTION_STR[ack.type],ENDL);

  Serial.print(OFFSET); btn(8,0,x); btn(9,0,c); Serial.println();
  Serial.print(OFFSET); btn(8,1,x); btn(9,1,c); Serial.printf("%s        Method:   %s%s",DEF,method,ENDL);
  Serial.print(OFFSET); btn(8,2,x); btn(9,2,c); Serial.printf("%s        Speed ↑↓: %d%s",DEF,speedFB,ENDL);
  Serial.print(OFFSET); btn(8,3,x); btn(9,3,c); Serial.printf("%s        Speed ←→: %d%s",DEF,speedLR,ENDL);

  ui_drawn = true;
}

void processInstruction() {
  // Clean up serial port (we want latest char only)
  last_char = 0;
  while(Serial.available()) last_char = toLowerCase(Serial.read());
  switch (last_char)
  {
    case 'x':
      next.type = InstructionType::StopMovement;
      next.data = 0;
      id++;
      break;
    case 'a':
      next.type = InstructionType::RotateLeft;
      next.data = speedLR;
      id++;
      break;
    case 'd':
      next.type = InstructionType::RotateRight;
      next.data = speedLR;
      id++;
      break;
    case 'w':
      next.type = InstructionType::MoveForward;
      next.data = speedFB;
      id++;
      break;
    case 's':
      next.type = InstructionType::MoveBackward;
      next.data = speedFB;
      id++;
      break;
    case 'c':
      next.type = InstructionType::ScanArea;
      next.data = speedLR;
      id++;
      break;
    case 'q':
        if(speedLR > 0) speedLR--;
        break;
    case 'e':
        if(speedLR < 15) speedLR++;
        break;
    case 'f':
        if(speedFB > 0) speedFB--;
        break;
    case 'r':
        if(speedFB < 15) speedFB++;
        break;
    default:
      if(next.type != InstructionType::StopMovement) {
        next.type = InstructionType::StopMovement;
        next.data = 0;
        id++;
      }
  };
  drawUI(false);
}

void laserTransmit() {
  Laser.emit(LOW);
  if(!Switches[0].State)  return;
  if(last_id == id) return;
  last_id = id;

  time start = micros();

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
  ack.type = value < 512 ? InstructionType::Ack : InstructionType::Invalid;

  time end = micros();
  // 1 byte / duration = N bytes / 1 second
  if(end - start > 0) optics.topSpeed = 1e6 / (end - start);
  
  Laser.emit(LOW);
}

void rfTransmit() {
  if(!Switches[1].State) return;
  if(last_id == id) return;
  last_id = id;
  //Serial.printf("Rf - %d. %s %d\n", id, INSTRUCTION_STR[next.type], next.data);
  
  u32 encoded;
  u8 len = next.encode(encoded);

  // Update display
  memcpy(rxTx.transmitBuff, INSTRUCTION_STR[next.type], MSG_BUFF_LEN);
  rxTx.transmitted = MSG_BUFF_LEN;
  displayUpdate();
  
  u32 resp = 0;
  u8 lenght = 0;
  
  time started = micros();
  time ended = 0;
  
  do {
    // Send data
    RfTransmitter.transmit(encoded, len);
    RfReceiver.reset();
    
    // Wait for confirmation
    lenght = RfReceiver.receive(resp);
    ended = micros();

    if(lenght == 8 && ack.decode(resp)) {
      drawUI(true);
    } else {
      drawUI(false);
    }
    Serial.flush();
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