#ifndef L_INSTRUCTION
#define L_INSTRUCTION

#include "../Shared/Shared.h"
#include "Device.h"

enum InstructionType {
  StopMovement,
  RotateLeft,
  RotateRight,
  MoveForward,
  MoveBackward,
  ScanArea,
};

extern const char* INSTRUCTION_STR[];

struct Instruction
{
  public:
    Device device;
    u8 order;
    InstructionType type;
    u8* data;
    u8 len;

    bool decode(u8* encoded, u8 encoded_len);

    u8 encode(u8* result);
};


#endif