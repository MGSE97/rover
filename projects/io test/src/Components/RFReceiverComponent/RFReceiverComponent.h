#ifndef LIB_RFR_COMPONENT
#define LIB_RFR_COMPONENT

#include <RCSwitch.h>
#include "Shared/Shared.h"

class RFReceiver: public HwComponent 
{
  public:
    pin Enable;
    pin Data;
    bool Enabled;

    RFReceiver(pin enable, pin data);
    ~RFReceiver();
    void init();
    void enable();
    void disable();
    u8 receive(u8& value);

  protected:
    RCSwitch Receiver;
};

#endif