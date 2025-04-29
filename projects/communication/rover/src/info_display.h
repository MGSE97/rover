#ifndef INFO_DISPLAY_H
#define INFO_DISPLAY_H

#include <clib/u8g.h>
#include "lib/Libs.h"

const u8 MSG_BUFF_LEN = 12;

enum Scenes {
  RxTxMsg,
  RfStats,
  OpticsStats
};

enum Method {
  RF,
  Optics
};

extern const char* METHOD_STR[];


struct RxTxData {
  u8 transmitBuff[MSG_BUFF_LEN];
  u8 receiveBuff[MSG_BUFF_LEN];
  u8 transmitted;
  u8 received;
};

struct StatsData {
  Method method;
  u32 topSpeed;
};

class InfoDisplay: public DisplayHwComponent {
  public:
    Scenes Scene;
    RxTxData* RxTx;
    StatsData* Rf;
    StatsData* Optics;

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
    u8 charWidth = 0;

    u8 transmitted[MSG_BUFF_LEN];
    u8 received[MSG_BUFF_LEN];
    
    void drawSceneRxTxMsg();
    void drawSceneRfStats();
    void drawSceneOpticsStats();

    void drawVar(Point position, const char* name, const char* value, const char* unit = "");
    void drawText(Point position, const char* text);
    
    void clear();
};

#endif