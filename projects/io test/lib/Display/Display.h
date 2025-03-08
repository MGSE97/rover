#pragma once
#include <Arduino.h>
#include <HwComponent.hpp>
#include <U8glib-HAL.h>

class DisplayHwComponent: public HwComponent 
{
  public:
    uint8_t RefreshMs;

    DisplayHwComponent(uint8_t refresh_ms = 25, uint8_t i2c_options = U8G_I2C_OPT_FAST);
    ~DisplayHwComponent();
    void init();
    bool shouldDraw();
    void draw();

  private:
    uint16_t LastDraw;

  protected:
    U8GLIB_SSD1306_128X32 Device;
    uint8_t Width;
    uint8_t Height;

    virtual void drawContent() {};
    virtual void beforeDraw() {};
    virtual void afterDraw() {};
};