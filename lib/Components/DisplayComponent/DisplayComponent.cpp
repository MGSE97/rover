#include "DisplayComponent.h"

DisplayHwComponent::DisplayHwComponent(u8 refresh_ms, u8 i2c_options) {
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
