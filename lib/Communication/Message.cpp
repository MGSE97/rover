#include "Message.h"

bool Message::decode(u32 encoded) {
  letter = encoded & 0xFF;
  order = (encoded >> 8) & 0xFF;
  sender = (Device)((encoded >> 16) & 0x03);
  return encoded > 0;
}

u8 Message::encode(u32& result) {
  result = (sender << 16) + (order << 8) + letter;
  return 18;
}