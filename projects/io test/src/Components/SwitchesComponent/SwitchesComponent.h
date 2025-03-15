#ifndef LIB_SWITCHES_COMPONENT
#define LIB_SWITCHES_COMPONENT

#include <Arduino.h>
#include "Shared/Shared.h"

struct Switch {
  PinDefinition Pin;
  bool State;
};

class SwitchesHwComponent : public HwComponent
{
  public:
  bool Changed;
  
  SwitchesHwComponent(pin pins[6]);
  
  Switch& operator[](u8 index) {
    return Items[index];
  }
  
  void init();
  void update();
  
  protected:
  Switch Items[6];
};

#endif