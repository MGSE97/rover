
#include <Arduino.h>
#include <Display.h>
#include <Utils.h>

const uint8_t CIRCLES = 5;

const uint8_t bitmapKey[] U8G_PROGMEM = {
  0b00111100,
  0b01100110,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00011100,
  0b00011100,
  0b00011111,
};

class PoweredOnDisplay: public DisplayHwComponent {
  public:
    PoweredOnDisplay(uint8_t refresh_ms = 25, uint8_t i2c_options = U8G_I2C_OPT_FAST)
      : DisplayHwComponent(refresh_ms, i2c_options) {}

    void init() {
      Device.setFont(u8g_font_timB12);
    }

  protected:
    void afterDraw() {
      size++;
      if(size > Width/2+CIRCLES*10) size = 0;
    }

    void drawContent() {
      // Border
      Device.setColorIndex(1);
      Device.drawFrame(0, 0, Width, Height);

      // Random points  
      for(char i = 0; i < 16; i++) {
        int x = random(0, Width);
        int y = random(0, Height);
        Device.setColorIndex(x > 0 && y > 0 && x < Width && y < Height);
        Device.drawPixel(x, y);
      }
      
      // Pulsing circles
      for(char i = 0; i < CIRCLES; i++) {
        char radius = size - 10 * i;
        if(radius > 0) Device.drawCircle(Width/2+4, Height/2, radius);
      }
      
      // Bitmap in center
      Device.drawBitmapP(Width/2, Height/2-4, 1, 8, bitmapKey);
      
      // Text with border
      uint8_t line = Device.getFontAscent() / 2;
      uint32_t time = millis()/1000;
      uint8_t s = 2;
      Device.setColorIndex(0);
      drawText(time, line, -s, -s);
      drawText(time, line, s, -s);
      drawText(time, line, -s, s);
      drawText(time, line, s, s);
      Device.setColorIndex(1);
      drawText(time, line, 0, 0);
    }

  private:
    uint8_t size = 0;

    void drawText(uint32_t time, uint8_t line, uint8_t dx, uint8_t dy) {
      Device.setPrintPos(2+dx, Height/2 + line+dy);
      Device.print("Powered");
      Device.setPrintPos(Width/2+14+dx, Height/2 + line+dy);
      Device.print(time);
      Device.print(" sec");
    }
};