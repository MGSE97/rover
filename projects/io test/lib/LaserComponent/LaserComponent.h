#pragma once
#include <Arduino.h>
#include <HwComponent.h>

class LaserHwComponent : public HwComponent
{
  private:
    volatile uint8_t* Reg;
    uint8_t BitMask;
  public:
    uint8_t Pin;
    uint8_t Emitting;

    LaserHwComponent(uint8_t pin);
    ~LaserHwComponent();

    void init();
    void emit(uint8_t value);
    void emitToggle();
};
