#ifndef LIB_RFR_COMPONENT
#define LIB_RFR_COMPONENT

#include <RCSwitch.h>
#include "../../Shared/Shared.h"

class RFReceiver: public HwComponent 
{
  public:
    pin Enable;
    pin Data;
    bool Enabled;
    u8 Protocol;

    RFReceiver(pin enable, pin data, u8 protocol = 1);
    ~RFReceiver();
    void init();
    void enable();
    void disable();
    void reset();
    u8 receive(u32& value);

  protected:
    RCSwitch Receiver;
};

#endif