#ifndef L_INSTRUCTION
#define L_INSTRUCTION

#include "../Shared/Shared.h"
#include "Device.h"

enum InstructionType {
  Invalid = 0,
  StopMovement = 1, // We skip 0 for transmission validation
  RotateLeft = 2,
  RotateRight = 3,
  MoveForward = 4,
  MoveBackward = 5,
  ScanArea = 6,
  Ack = 7,
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