#pragma once
#include <Arduino.h>
#include <HwComponent.hpp>

class MotorHwComponent : public HwComponent
{
  public:
    uint8_t LeftPinH;
    uint8_t LeftPinL;

    MotorHwComponent(uint8_t left_pin_h, int8_t left_pin_l) {
      LeftPinH = left_pin_h;
      LeftPinL = left_pin_l;
    }

    ~MotorHwComponent() {
      digitalWrite(LeftPinH, LOW);
      digitalWrite(LeftPinL, LOW);
    }

    void init() {
      pinMode(LeftPinH, OUTPUT);
      pinMode(LeftPinL, OUTPUT);
      digitalWrite(LeftPinH, LOW);
      digitalWrite(LeftPinL, LOW);
    }

    void drive(uint8_t value) {
      digitalWrite(LeftPinH, value ? HIGH : LOW);
      digitalWrite(LeftPinL, LOW);
    }
  
  private:
    volatile uint8_t* Reg;
    uint8_t BitMask;
};
