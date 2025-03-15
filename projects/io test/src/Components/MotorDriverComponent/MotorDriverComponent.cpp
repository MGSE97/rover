#include "MotorDriverComponent.h"

const char* DIRECTION_STR[] = {
  "Left", 
  "Right", 
  "Forward", 
  "Backward", 
  "Stop"
};

MotorDriverHwComponent::MotorDriverHwComponent(MotorPins left, MotorPins right, u8 stop_time, u8 spin_time) {
  Left = left;
  Right = right;
  CurrentSpeed = 0;
  CurrentDirection = Stop;
  StopTime = stop_time;
  SpinTime = spin_time;
}

MotorDriverHwComponent::~MotorDriverHwComponent() {
  drop(Left);
  drop(Right);
}

void MotorDriverHwComponent::init() {
  init(Left);
  init(Right);
}

void MotorDriverHwComponent::drive(Direction direction, u8 speed) {
  now = millis();
  long int elapsed = now - lastStarted;
  
  // In case of change direction, we:
  //  1. wait a while for wheel to stop
  //  2. Spin wheel with full power to break friction
  //  3. Spin wheel with specified speed  
  u8 safe_speed = 0;
  if(direction != CurrentDirection) {
    lastStarted = now;
    safe_speed = 0;
  } else if(elapsed > StopTime + SpinTime) {
    safe_speed = speed;
  } else if(elapsed > StopTime) {
    safe_speed = 100;
  } else {
    safe_speed = 0;
  }

  u8 power = map(safe_speed, 0, 100, 50, 255);
  Teleplot.sendInt("Direction", direction);
  Teleplot.sendInt("Power", power);
  Teleplot.sendInt("Speed", speed);

  switch (direction)
  {
    case Direction::Forward:
      set(Left, power, false);
      set(Right, power, false);
      break;        
    case Direction::Backward:
      set(Left, power, true);
      set(Right, power, true);
      break;
    case Direction::Left:
      set(Left, power, false);
      set(Right, power, true);
      break;
    case Direction::Right:
      set(Left, power, true);
      set(Right, power, false);
      break;
    case Direction::Stop:
      set(Left, 0, false);
      set(Right, 0, false);
      break;
  }

  CurrentDirection = direction;
  CurrentSpeed = speed;
}

void MotorDriverHwComponent::init(MotorPins side) {
  pinMode(side.High, OUTPUT);
  pinMode(side.Low, OUTPUT);
  pinMode(side.Power, OUTPUT);
  digitalWrite(side.High, LOW);
  digitalWrite(side.Low, LOW);
  analogWrite(side.Power, 0);
}

void MotorDriverHwComponent::drop(MotorPins side) {
  digitalWrite(side.High, LOW);
  digitalWrite(side.Low, LOW);
  analogWrite(side.Power, 0);
}

void MotorDriverHwComponent::set(MotorPins side, u8 power, bool swap) {
  digitalWrite(side.High, swap ? LOW : HIGH);
  digitalWrite(side.Low, swap ? HIGH : LOW);
  analogWrite(side.Power, power);
}
