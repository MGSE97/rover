
#include "info_display.h"

const u8 CIRCLES = 5;

const u8 bitmapKey[] U8G_PROGMEM = {
  0b00111100,
  0b01100110,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00011100,
  0b00011100,
  0b00011111,
};

void InfoDisplay::init() {
  Device.setFont(u8g_font_timB12);
  lineHeight = Device.getFontAscent();
  fullLineHeight = Device.getFontLineSpacing();
}

void InfoDisplay::afterDraw() {
  if(Scene == PoweredOnStats) {
    size++;
    if(size > Width/2+CIRCLES*10) size = 0;
  }
}

void InfoDisplay::drawContent() {
  switch (Scene)
  {
    case PoweredOnStats:
      drawScenePoweredOnStats();
      break;
    case LightSensorStatus:
      drawSceneLightSensorStatus();
      break;
    case DistanceSensorStatus:
      drawSceneDistanceSensorStatus();
      break;
    case MotorDriverStatus:
      drawSceneMotorDriverStatus();
      break;
    case SwitchesStatus:
      drawSceneSwitchStatus();
      break;
  }
}

void InfoDisplay::drawSceneSwitchStatus() {
  if(Switch == nullptr) return;
  
  char status[7];
  for(u8 i = 0; i < 6; i++) status[i] = Switch->values[i] ? '1' : '0';
  status[7] = '\0';
  
  Device.setColorIndex(1);
  Point position = {2, Height - lineHeight/2};
  drawVar(position, "Switch", status);
}

void InfoDisplay::drawSceneLightSensorStatus() {
  if(LightSensors == nullptr) return;
  
  Device.setColorIndex(1);
  String value1 = String(LightSensors->value1);
  String value2 = String(LightSensors->value2);
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position, "Light 1", value1.c_str());
  drawVar(position + nextLine, "Light 2", value2.c_str());
}


void InfoDisplay::drawSceneDistanceSensorStatus() {
  if(DistanceSensor == nullptr) return;
  
  Device.setColorIndex(1);
  String distance = String(DistanceSensor->distance);
  String duration = String(DistanceSensor->duration);
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position,            "Duration", duration.c_str(), "us");
  drawVar(position + nextLine, "Distance", distance.c_str(), "cm");
}

void InfoDisplay::drawSceneMotorDriverStatus() {
  if(MotorDriver == nullptr) return;
  
  Device.setColorIndex(1);
  String direction = String(DIRECTION_STR[MotorDriver->direction]);
  String speed = String(MotorDriver->speed);
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position, "Direction", direction.c_str());
  drawVar(position + nextLine, "Speed", speed.c_str(), "%");
}

void InfoDisplay::drawScenePoweredOnStats() {
  // Border
  Device.setColorIndex(1);
  Device.drawFrame(0, 0, Width, Height);

  // Random points  
  for(char i = 0; i < 16; i++) {
    u8 x = randomInRange(0, Width);
    u8 y = randomInRange(0, Height);
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
  int32_t border = 2;
  Point powered = {2, Height/2 + lineHeight/2};
  const char* poweredText = "Powered";
  Point value = {Width/2+10, Height/2 + lineHeight/2};
  String valueText = String(millis()/1000);
  Device.setColorIndex(0);
  drawText(powered + Point{-border, -border}, poweredText);
  drawText(powered + Point{ border, -border}, poweredText);
  drawText(powered + Point{-border,  border}, poweredText);
  drawText(powered + Point{ border,  border}, poweredText);
  Device.setColorIndex(1);
  drawText(powered, poweredText);
  drawText(value, valueText.c_str());
  Device.print(" sec");
}

void InfoDisplay::drawVar(Point position, const char* name, const char* value, const char* unit) {
  Device.setPrintPos(position.X, position.Y);
  Device.print(name);
  Device.print(": ");
  Device.print(value);
  Device.print(" ");
  Device.print(unit);
}

void InfoDisplay::drawText(Point position, const char* text) {
  Device.setPrintPos(position.X, position.Y);
  Device.print(text);
}