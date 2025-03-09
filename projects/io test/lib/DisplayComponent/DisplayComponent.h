#pragma once
#include <Arduino.h>
#include <HwComponent.h>
#include <U8glib-HAL.h>

struct Point
{
  int32_t X;
  int32_t Y;

  Point operator+(Point a) {
    return {X+a.X, Y+a.Y};
  }

  Point operator-(Point a) {
    return {X+a.X, Y+a.Y};
  }

  Point operator*(Point a) {
    return {X+a.X, Y+a.Y};
  }

  Point operator/(Point a) {
    return {X+a.X, Y+a.Y};
  }
  

  Point operator+(int a) {
    return {X+a, Y+a};
  }

  Point operator-(int a) {
    return {X-a, Y-a};
  }

  Point operator*(int a) {
    return {X*a, Y*a};
  }

  Point operator/(int a) {
    return {X/a, Y/a};
  }

  
  Point& operator+=(Point a) {
    X += a.X;
    Y += a.Y;
    return *this;
  }

  Point& operator-=(Point a) {
    X -= a.X;
    Y -= a.Y;
    return *this;
  }

  Point& operator*=(Point a) {
    X *= a.X;
    Y *= a.Y;
    return *this;
  }

  Point& operator/=(Point a) {
    X /= a.X;
    Y /= a.Y;
    return *this;
  }
};


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