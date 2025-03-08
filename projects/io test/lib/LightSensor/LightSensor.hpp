#pragma once
#include <Arduino.h>
#include <HwComponent.hpp>
#include <Utils.h>

class LightSensorHwComponent : public HwComponent
{
  public:
    uint8_t Pin;

    LightSensorHwComponent(uint8_t pin) {
      Pin = pin;
    }

    ~LightSensorHwComponent() {
      digitalWrite(Pin, LOW);
    }

    void init() {
      pinMode(Pin, OUTPUT);
      digitalWrite(Pin, LOW);
      pinMode(Pin, INPUT);

      // Set ADC prescale to 16
      sbi(ADCSRA,ADPS2);
      cbi(ADCSRA,ADPS1);
      cbi(ADCSRA,ADPS0);
    }

    uint16_t receive() {
      return analogRead(Pin);
    }
};
