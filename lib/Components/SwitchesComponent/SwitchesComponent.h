#ifndef LIB_SWITCHES_COMPONENT
#define LIB_SWITCHES_COMPONENT

#include <Arduino.h>
#include "Shared/Shared.h"

class SwitchesHwComponent : public HwComponent
{
  public:
  bool Changed;
  
  SwitchesHwComponent(pin read, pin clk, pin reset);
  
  bool operator[](u8 index) {
    return States[index];
  }
  
  void init();
  void update();
  
  protected:
    bool States[6];
    PinDefinition Read;
    PinDefinition Clk;
    PinDefinition Reset;
};

#endif