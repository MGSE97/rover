#ifndef LIB_MOTOR_DRIVER_COMPONENT
#define LIB_MOTOR_DRIVER_COMPONENT

#include <Arduino.h>
#include "../../Shared/Shared.h"

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

enum MotorState {
  Stopped,
  SpinUp,
  Running,
  Stopping
};

extern const char* DIRECTION_STR[];


class MotorDriverHwComponent : public HwComponent
{
  public:
    MotorPins Left;
    MotorPins Right;
    u8 TargetSpeed;
    u8 CurrentSpeed;
    Direction TargetDirection;
    Direction CurrentDirection;
    time TargetDuration;
    time StopTime;
    time SpinTime;
    u8 MaxSpeed;
    u8 SpinSpeed;

     MotorDriverHwComponent(MotorPins left, MotorPins right, time stop_time = 100, time spin_time = 100, u8 max_speed = 100, u8 spin_speed = 80);
    ~MotorDriverHwComponent();

    void init();
    void update();
    void drive(Direction direction, u8 speed, time duration);
  
  private:
    time lastMovement = 0;
    MotorState state;

    void init(MotorPins side);
    void drop(MotorPins side);
    void set(MotorPins side, u8 power, bool swap);
};

#endif