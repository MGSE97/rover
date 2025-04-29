#ifndef LIB_LIGHT_SENSOR_COMPONENT
#define LIB_LIGHT_SENSOR_COMPONENT

#include <Arduino.h>
#include "../../Shared/Shared.h"

class LightSensorHwComponent : public HwComponent
{
  public:
    pin Pin;

    LightSensorHwComponent(pin pin);

    void init();
    u16 receive();
};

#endif