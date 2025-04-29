#ifndef LIB_DISTANCE_COMPONENT
#define LIB_DISTANCE_COMPONENT

#include <Arduino.h>
#include "../../Shared/Shared.h"

class DistanceHwComponent : public HwComponent
{
  public:
    DistanceHwComponent(pin trigger, pin echo);

    void init();
    
    void stop();
    bool measure(double& distance, time& time_us);
  
  protected:
    pin Echo;
    PinDefinition Trigger;
    time Emit = 0;
    bool Emitting = false;
};

#endif