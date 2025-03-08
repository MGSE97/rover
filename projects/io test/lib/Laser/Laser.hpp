#pragma once
#include <Arduino.h>
#include <HwComponent.hpp>

class LaserHwComponent : public HwComponent
{
  private:
    volatile uint8_t* Reg;
    uint8_t BitMask;
  public:
    uint8_t Pin;
    uint8_t Emitting;

    LaserHwComponent(uint8_t pin) {
      Pin = pin;
      Reg = portOutputRegister(digitalPinToPort(pin));
      BitMask = digitalPinToBitMask(pin);
      Emitting = false;
    }

    ~LaserHwComponent() {
      digitalWrite(Pin, LOW);
    }

    void init() {
      pinMode(Pin, OUTPUT);
      digitalWrite(Pin, LOW);
    }

    void emit(uint8_t value) {
      Emitting = value;
      // This is faster than digitalWrite function, but less safe
      if(value) *Reg |= BitMask;
      else      *Reg &= ~BitMask;
    }

    void emitToggle() {
      emit(!Emitting);
    }
};
