#ifndef L_MESSAGE
#define L_MESSAGE

#include "Device.h"
#include "../Shared/Shared.h"

struct Message {
  Device sender;
  u8 order;
  u8 letter;

  
  bool decode(u32 encoded);

  u8 encode(u32& result);
};

#endif