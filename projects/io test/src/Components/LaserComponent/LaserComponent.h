#ifndef LIB_LASER_COMPONENT
#define LIB_LASER_COMPONENT

#include <Arduino.h>
#include "Shared/Shared.h"

class LaserHwComponent : public HwComponent
{
  private:
    PinDefinition Pin;
  public:
    u8 Emitting;

    LaserHwComponent(pin pin);
    ~LaserHwComponent();

    void init();
    void emit(u8 value);
    void emitToggle();
};

#endif