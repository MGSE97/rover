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
    InstructionType type;
    u8 data;

    bool decode(u32 encoded);

    u8 encode(u32& result);
};


#endif