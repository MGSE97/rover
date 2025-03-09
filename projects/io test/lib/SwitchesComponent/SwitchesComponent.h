#pragma once
#include <Arduino.h>
#include <HwComponent.h>

struct Switch {
  uint8_t Pin;
  bool State;
};

class SwitchesHwComponent : public HwComponent
{
  public:
    bool Changed;

    SwitchesHwComponent(uint8_t ports[6]);

    Switch& operator[](uint8_t index) {
      return Items[index];
    }

    void init();
    void update();
  
  protected:
    Switch Items[6];
};