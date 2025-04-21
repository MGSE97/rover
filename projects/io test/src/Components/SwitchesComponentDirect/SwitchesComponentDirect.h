#ifndef LIB_SWITCHES_COMPONENT_DIRECT
#define LIB_SWITCHES_COMPONENT_DIRECT

#include <Arduino.h>
#include "Shared/Shared.h"

struct Switch {
  PinDefinition Pin;
  bool State;
};

class SwitchesHwComponentDirect : public HwComponent
{
  public:
  bool Changed;
  
  SwitchesHwComponentDirect(pin pins[6]);
  
  Switch& operator[](u8 index) {
    return Items[index];
  }
  
  void init();
  void update();
  
  protected:
  Switch Items[6];
};

#endif