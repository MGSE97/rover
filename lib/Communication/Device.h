#ifndef DEVICE
#define DEVICE

#define AsController(x) x | 0x100
#define AsRover(x) x | 0x101
#define GetData(x) x & 0xFF
#define IsValid(x, id) x & (0x100+id) = (0x101+id)

enum Device {
  Controller,
  Rover
};


extern const char* DEVICE_STR[];


#endif