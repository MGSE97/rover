#include "Instruction.h"

const char* INSTRUCTION_STR[] = {
  "Stop",
  "Rotate Left",
  "Rotate Right",
  "Move Forward",
  "Move Backward",
  "Scan"
};


bool Instruction::decode(u8* encoded, u8 encoded_len) {
  if(encoded_len == 0) return false;

  // Decode first byte
  device = (Device)(encoded[0] & 0b10000000);
  type = (InstructionType)(encoded[0] & 0b01110000);
  order = encoded[0] & 0b00001111;

  // Copy rest into instruction buffer
  if(encoded_len > 1) {
    len = encoded_len - 1;
    for(u8 i = 0; i < len; i++) data[i] = encoded[i+1];
  }
}

u8 Instruction::encode(u8* result) {
  result[0] = (device << 7) + (type << 4) + (order & 0b1111);
  for(u8 i = 0; i < len; i++) result[i+1] = data[i];

  return len + 1;
}