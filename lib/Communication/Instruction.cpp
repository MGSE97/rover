#include "Instruction.h"

const char* INSTRUCTION_STR[] = {
  "Stop",
  "Left",
  "Right",
  "Forward",
  "Backward",
  "Scan"
};


bool Instruction::decode(u32 encoded) {
  type = (InstructionType)((encoded & 0xF0) >> 4);
  data = encoded & 0xF;
  return encoded > 0xF;
}

u8 Instruction::encode(u32& result) {
  result = (type << 4) + (data & 0xF);
  return 8;
}