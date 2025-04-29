#include "Message.h"

bool Message::decode(u32 encoded) {
  letter = encoded & 0xFF;
  order = (encoded >> 8) & 0x3F;
  sender = (Device)((encoded >> 14) & 0x3);
  return encoded > 0x7FFF;
}

u8 Message::encode(u32& result) {
  result = (sender << 14) + ((order & 0x3F) << 8) + letter;
  return 16;
}