#pragma once
#include <Arduino.h>
#include <HwComponent.h>
#include <Utils.h>

struct MotorPins
{
  uint8_t High;
  uint8_t Low;
  /// Pin with PWM
  uint8_t Power;
};

enum Direction {Left, Right, Forward, Backward, Stop};


class MotorDriverHwComponent : public HwComponent
{
  public:
    MotorPins Left;
    MotorPins Right;
    uint8_t CurrentSpeed;
    Direction CurrentDirection;

     MotorDriverHwComponent(MotorPins left, MotorPins right, uint8_t stop_time = 100, uint8_t spin_time = 100);
    ~MotorDriverHwComponent();

    void init();
    void drive(Direction direction, uint8_t speed);
  
  private:
    uint8_t StopTime;
    uint8_t SpinTime;
    long int lastStarted = 0;
    long int now = 0;

    void init(MotorPins side);
    void drop(MotorPins side);
    void set(MotorPins side, uint8_t power, bool swap);
};
