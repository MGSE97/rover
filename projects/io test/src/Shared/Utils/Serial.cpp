#include "Serial.h"

size_t TeleplotSerial::sendText(const char* name, char* value) {
  return Serial.printf(">%s:%s\n", name, value);
}

size_t TeleplotSerial::sendText(const char* name, char* value, char* unit) {
  return Serial.printf(">%s:%s§%s\n", name, value, unit);
}

size_t TeleplotSerial::sendInt(const char* name, signed long value) {
  return Serial.printf(">%s:%ld\n", name, value);
}

size_t TeleplotSerial::sendInt(const char* name, signed long value, char* unit) {
  return Serial.printf(">%s:%ld§%s\n", name, value, unit);
}

size_t TeleplotSerial::sendUInt(const char* name, unsigned long value) {
  return Serial.printf(">%s:%lu\n", name, value);
}

size_t TeleplotSerial::sendUInt(const char* name, unsigned long value, char* unit) {
  return Serial.printf(">%s:%lu§%s\n", name, value, unit);
}

size_t TeleplotSerial::sendDouble(const char* name, double value) {
  String number = String(value);
  return Serial.printf(">%s:%s\n", name, number.c_str());
}

size_t TeleplotSerial::sendDouble(const char* name, double value, char* unit) {
  String number = String(value);
  return Serial.printf(">%s:%s§%s\n", name, number.c_str(), unit);
}

size_t TeleplotSerial::debug(const char* name, int value) {
  return Serial.printf("%s: %x\n", name, value);
}

size_t TeleplotSerial::debug(const char* name, int value, char* unit) {
  return Serial.printf("%s: %x %s\n", name, value, unit);
}