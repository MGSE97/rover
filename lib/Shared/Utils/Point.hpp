#include <Arduino.h>

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