#pragma once
#include <DisplayComponent.h>
#include <MotorDriverComponent.h>
#include <Utils.h>
#include <clib/u8g.h>

enum Scenes {PoweredOnStats, LightSensorStatus, MotorDriverStatus, SwitchesStatus};

struct LightSensorsData {
  uint16_t value1;
  uint16_t value2;
};

struct SwitchesData {
  bool values[6];
};

struct MotorDriverData {
  Direction direction;
  uint8_t speed;
};

class InfoDisplay: public DisplayHwComponent {
  public:
    Scenes Scene;
    LightSensorsData* LightSensors;
    MotorDriverData* MotorDriver;
    SwitchesData* Switch;

    inline InfoDisplay(uint8_t refresh_ms = 25, uint8_t i2c_options = U8G_I2C_OPT_FAST)
      : DisplayHwComponent(refresh_ms, i2c_options) {}

    void init();

  protected:
    void afterDraw();

    void drawContent();

  private:
    uint8_t size = 0;
    uint8_t lineHeight = 0;
    uint8_t fullLineHeight = 0;
    
    void drawSceneSwitchStatus();

    void drawSceneLightSensorStatus();

    void drawSceneMotorDriverStatus();

    void drawScenePoweredOnStats();

    void drawVar(Point position, const char* name, const char* value);

    void drawText(Point position, const char* text);
};