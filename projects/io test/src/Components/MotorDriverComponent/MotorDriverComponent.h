#ifndef LIB_MOTOR_DRIVER_COMPONENT
#define LIB_MOTOR_DRIVER_COMPONENT

#include <Arduino.h>
#include "Shared/Shared.h"

struct MotorPins
{
  pin High;
  pin Low;
  /// Pin with PWM
  pin Power;
};

enum Direction {
  Left, 
  Right, 
  Forward, 
  Backward, 
  Stop
};

extern const char* DIRECTION_STR[];


class MotorDriverHwComponent : public HwComponent
{
  public:
    MotorPins Left;
    MotorPins Right;
    u8 CurrentSpeed;
    Direction CurrentDirection;

     MotorDriverHwComponent(MotorPins left, MotorPins right, u8 stop_time = 100, u8 spin_time = 100);
    ~MotorDriverHwComponent();

    void init();
    void drive(Direction direction, u8 speed);
  
  private:
    u8 StopTime;
    u8 SpinTime;
    long int lastStarted = 0;
    long int now = 0;

    void init(MotorPins side);
    void drop(MotorPins side);
    void set(MotorPins side, u8 power, bool swap);
};

#endif