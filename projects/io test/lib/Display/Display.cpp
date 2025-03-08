#include "Display.h"

DisplayHwComponent::DisplayHwComponent(uint8_t refresh_ms, uint8_t i2c_options) {
  Device = U8GLIB_SSD1306_128X32(i2c_options);
  Width = Device.getWidth();
  Height = Device.getHeight();
  RefreshMs = refresh_ms;
  LastDraw = millis();
}

DisplayHwComponent::~DisplayHwComponent() {}

void DisplayHwComponent::init() {}

bool DisplayHwComponent::shouldDraw() {
  return millis() - LastDraw > RefreshMs;
}

void DisplayHwComponent::draw() {
  beforeDraw();
  Device.firstPage();
  do {
    drawContent();
  } while(Device.nextPage());
  afterDraw();
  LastDraw = millis();
}
