#ifndef INFO_DISPLAY_H
#define INFO_DISPLAY_H

#include <clib/u8g.h>
#include "Libs.h"

enum Scenes {
  PoweredOnStats, 
  LightSensorStatus,
  MotorDriverStatus,
  SwitchesStatus,
  DistanceSensorStatus
};

struct LightSensorsData {
  u16 value1;
  u16 value2;
};

struct DistanceSensorsData {
  double distance;
  time duration;
};

struct SwitchesData {
  bool values[6];
};

struct MotorDriverData {
  Direction direction;
  u8 speed;
};

class InfoDisplay: public DisplayHwComponent {
  public:
    Scenes Scene;
    LightSensorsData* LightSensors;
    MotorDriverData* MotorDriver;
    SwitchesData* Switch;
    DistanceSensorsData* DistanceSensor;

    inline InfoDisplay(u8 refresh_ms = 25, u8 i2c_options = U8G_I2C_OPT_FAST)
      : DisplayHwComponent(refresh_ms, i2c_options) {}

    void init();

  protected:
    void afterDraw();

    void drawContent();

  private:
    u8 size = 0;
    u8 lineHeight = 0;
    u8 fullLineHeight = 0;
    
    void drawSceneSwitchStatus();

    void drawSceneLightSensorStatus();

    void drawSceneMotorDriverStatus();

    void drawSceneDistanceSensorStatus();

    void drawScenePoweredOnStats();

    void drawVar(Point position, const char* name, const char* value, const char* unit = "");

    void drawText(Point position, const char* text);
};

#endif