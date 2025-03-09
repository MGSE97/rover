#pragma once
#include <Arduino.h>
#include <HwComponent.h>
#include <Utils.h>

class LightSensorHwComponent : public HwComponent
{
  public:
    uint8_t Pin;

    LightSensorHwComponent(uint8_t pin);
    ~LightSensorHwComponent();

    void init();
    uint16_t receive();
};
