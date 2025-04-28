#ifndef LIB_DISPLAY_COMPONENT
#define LIB_DISPLAY_COMPONENT

#include <Arduino.h>
#include <U8glib-HAL.h>
#include "Shared/Shared.h"


class DisplayHwComponent: public HwComponent 
{
  public:
    u8 RefreshMs;

    DisplayHwComponent(u8 refresh_ms = 25, u8 i2c_options = U8G_I2C_OPT_FAST);
    ~DisplayHwComponent();
    void init();
    bool shouldDraw();
    void draw();

  private:
    u16 LastDraw;

  protected:
    U8GLIB_SSD1306_128X32 Device;
    u8 Width;
    u8 Height;

    virtual void drawContent() {};
    virtual void beforeDraw() {};
    virtual void afterDraw() {};
};

#endif