#ifndef LIB_RFT_COMPONENT
#define LIB_RFT_COMPONENT

#include <RCSwitch.h>
#include "../../Shared/Shared.h"

class RFTransmitter: public HwComponent 
{
  public:
    pin Data;

    RFTransmitter(pin data, u8 protocol = 1);
    ~RFTransmitter();
    void init();
    void transmit(u8 value, u8 lengh);

  protected:
    RCSwitch Transmitter;
    bool Enabled;
};

#endif