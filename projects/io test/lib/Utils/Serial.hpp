#pragma once
#include <Arduino.h>

class TeleplotSerial {
  public:
    TeleplotSerial(){}

    size_t send(char* name, char* value) {
      Serial.printf(">%s:%s\n", name, value);
    }
  
    size_t send(char* name, char* value, char* unit) {
      Serial.printf(">%s:%s§%s\n", name, value, unit);
    }

    size_t send(char* name, int value) {
      Serial.printf(">%s:%d\n", name, value);
    }
  
    size_t send(char* name, int value, char* unit) {
      Serial.printf(">%s:%d§%s\n", name, value, unit);
    }

    size_t send(char* name, double value) {
      Serial.printf(">%s:%f\n", name, value);
    }
  
    size_t send(char* name, double value, char* unit) {
      Serial.printf(">%s:%f§%s\n", name, value, unit);
    }

    size_t debug(char* name, int value) {
      Serial.printf("%s: %x\n", name, value);
    }
  
    size_t debug(char* name, int value, char* unit) {
      Serial.printf("%s: %x %s\n", name, value, unit);
    }
};

TeleplotSerial Teleplot;

void t() {
  Teleplot.send("test", 4);
}