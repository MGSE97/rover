#include "Serial.h"

size_t TeleplotSerial::sendText(const char* name, char* value) {
  return Serial.printf(">%s:%s\n", name, value);
}

size_t TeleplotSerial::sendText(const char* name, char* value, char* unit) {
  return Serial.printf(">%s:%s§%s\n", name, value, unit);
}

size_t TeleplotSerial::sendInt(const char* name, int value) {
  return Serial.printf(">%s:%d\n", name, value);
}

size_t TeleplotSerial::sendInt(const char* name, int value, char* unit) {
  return Serial.printf(">%s:%d§%s\n", name, value, unit);
}

size_t TeleplotSerial::sendDouble(const char* name, double value) {
  return Serial.printf(">%s:%f\n", name, value);
}

size_t TeleplotSerial::sendDouble(const char* name, double value, char* unit) {
  return Serial.printf(">%s:%f§%s\n", name, value, unit);
}

size_t TeleplotSerial::debug(const char* name, int value) {
  return Serial.printf("%s: %x\n", name, value);
}

size_t TeleplotSerial::debug(const char* name, int value, char* unit) {
  return Serial.printf("%s: %x %s\n", name, value, unit);
}