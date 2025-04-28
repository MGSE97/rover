#ifndef LIB_DISTANCE_2D_COMPONENT
#define LIB_DISTANCE_2D_COMPONENT

#include <Arduino.h>
#include "Shared/Shared.h"

class Distance2DHwComponent : public HwComponent
{
  public:
    // @param distance Distance between sensors in mm.
    Distance2DHwComponent(pin trigger, pin echoLeft, pin echoRight, u8 distance = 52);

    void init();
    
    void stop();
    bool measure(Point &position);
  
  protected:
    PinDefinition EchoLeft;
    PinDefinition EchoRight;
    PinDefinition Trigger;
    time Emit = 0;
    bool Emitting = false;
    bool LeftUp = false;
    bool RightUp = false;
    time MeasureStarted = 0;
    time MeasureEnded = 0;
    time Left = 0;
    time Right = 0;
    u8 Distance;
};

extern time DistanceTimeout;

#endif