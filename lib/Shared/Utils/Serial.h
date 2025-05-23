#include <Arduino.h>

class TeleplotSerial {
  public:
    size_t sendText(const char* name, char* value);
    size_t sendText(const char* name, char* value, char* unit);

    size_t sendInt(const char* name, long value);
    size_t sendInt(const char* name, long value, char* unit);

    size_t sendUInt(const char* name, unsigned long value);
    size_t sendUInt(const char* name, unsigned long value, char* unit);

    size_t sendDouble(const char* name, double value);
    size_t sendDouble(const char* name, double value, char* unit);

    size_t debug(const char* name, int value);
    size_t debug(const char* name, int value, char* unit);
};

extern TeleplotSerial Teleplot;