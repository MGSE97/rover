
#include "info_display.h"

const char* METHOD_STR[] = {
  "RF",
  "Optics"
};

void InfoDisplay::init() {
  Device.setFont(u8g_font_timB12);
  lineHeight = Device.getFontAscent();
  fullLineHeight = Device.getFontLineSpacing();
  charWidth = Device.getStrPixelWidth("_");
}

void InfoDisplay::afterDraw() {
}

void InfoDisplay::drawContent() {
  switch (Scene)
  {
    case RxTxMsg:
      drawSceneRxTxMsg();
      break;
    case RfStats:
      drawSceneRfStats();
      break;
    case OpticsStats:
      drawSceneOpticsStats();
      break;
  }
}

void InfoDisplay::drawSceneRxTxMsg() {
  if(RxTx == nullptr) return;
  
  // Clean up buffers
  memcpy(transmitted, RxTx->transmitBuff, RxTx->transmitted);
  for(u8 i = RxTx->transmitted; i < 10; i++) transmitted[i] = 0;
  
  memcpy(received, RxTx->receiveBuff, RxTx->received);
  for(u8 i = RxTx->received; i < 10; i++) received[i] = 0;
  
  // Draw data
  clear();
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position,            "TX", (const char*)transmitted);
  drawVar(position + nextLine, "RX", (const char*)received);
}

void InfoDisplay::drawSceneRfStats() {
  if(Rf == nullptr) return;
  
  clear();
  String top = String(METHOD_STR[Rf->method]);
  String bottom = String(Rf->topSpeed);
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position, "Method", top.c_str());
  drawVar(position + nextLine, "Speed", bottom.c_str(), "B/s");
}

void InfoDisplay::drawSceneOpticsStats() {
  if(Optics == nullptr) return;
  
  clear();
  String top = String(METHOD_STR[Optics->method]);
  String bottom = String(Optics->topSpeed);
  Point position = {2, Height - fullLineHeight};
  Point nextLine = {0, fullLineHeight};
  drawVar(position, "Method", top.c_str());
  drawVar(position + nextLine, "Speed", bottom.c_str(), "B/s");
}

void InfoDisplay::drawVar(Point position, const char* name, const char* value, const char* unit) {
  Device.setPrintPos(position.X, position.Y);
  Device.print(name);
  Device.print(": ");
  Device.print(value);
  Device.print(" ");
  Device.print(unit);
}

void InfoDisplay::clear() {
  Device.setColorIndex(0);
  Device.drawBox(0,0,Width,Height);
  Device.setColorIndex(1);
}