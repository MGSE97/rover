#include "Instruction.h"

const char* INSTRUCTION_STR[] = {
  "Invalid",
  "Stop",
  "Left",
  "Right",
  "Forward",
  "Backward",
  "Scan",
  "Ack"
};


bool Instruction::decode(u32 encoded) {
  // Skip unknown instruction
  u8 instruction = (encoded & 0xF0) >> 4;
  if(instruction == 0 || instruction > InstructionType::Ack) return false;

  type = (InstructionType)instruction;
  data = encoded & 0xF;
  return encoded > 0xF;
}

u8 Instruction::encode(u32& result) {
  result = (type << 4) + (data & 0xF);
  return 8;
}