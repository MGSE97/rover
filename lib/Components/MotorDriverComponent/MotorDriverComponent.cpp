#include "MotorDriverComponent.h"

const char* DIRECTION_STR[] = {
  "Left", 
  "Right", 
  "Forward", 
  "Backward", 
  "Stop"
};

MotorDriverHwComponent::MotorDriverHwComponent(MotorPins left, MotorPins right, time stop_time, time spin_time, u8 max_speed, u8 spin_speed) {
  Left = left;
  Right = right;
  CurrentSpeed = 0;
  CurrentDirection = Stop;
  TargetSpeed = 0;
  TargetDirection = Stop;
  TargetDuration = 0;
  StopTime = stop_time;
  SpinTime = spin_time;
  MaxSpeed = max_speed;    
  SpinSpeed = spin_speed;
  state = Stopped;
}

MotorDriverHwComponent::~MotorDriverHwComponent() {
  drop(Left);
  drop(Right);
}

void MotorDriverHwComponent::init() {
  init(Left);
  init(Right);
}

void MotorDriverHwComponent::drive(Direction direction, u8 speed, time duration) {
  TargetSpeed = map(speed, 0, 100, 0, MaxSpeed);
  TargetDirection = direction;
  TargetDuration = duration;
}


/**
 * Updates motor speed and direction based on target.
 *
 * General rules:
 *  1. Wait a while for wheel to stop
 *  2. Spin wheel with full power to break friction
 *  3. Spin wheel with specified speed  
 * 
 * Implementation overview:
 *  1. Moving at target speed and direction
 *    - skip
 *  2. Rover is moving
 *    A. Same direction
 *       * Spinned up (after SpinTime)
 *          - update speed
 *       * Moved long enough
 *          - stop 
 *    B. Different direction
 *       * Stop
 *          - set speed to 0
 *  3. Rover is not moving (speed is 0)
 *    * Stopped (after StopTime)
 *      - update direction
 *      - spin up (speed to MaxSpeed)
 */
void MotorDriverHwComponent::update() {
  time now = millis();
  time elapsed = now - lastMovement;

  switch (state)
  {
    case Stopped:
      if(TargetSpeed != 0) {
        state = SpinUp;
        CurrentSpeed = SpinSpeed;
        CurrentDirection = TargetDirection;
        lastMovement = now;
      }
      break;
    case SpinUp:
      if(elapsed >= SpinTime) {
        state = Running;
        CurrentSpeed = TargetSpeed;
        lastMovement = now;
      }
      break;
    case Running:
      if(CurrentDirection != TargetDirection || elapsed >= TargetDuration) {
        state = Stopping;
        CurrentSpeed = 0;
        TargetSpeed = 0;
        lastMovement = now;
      } else if(CurrentSpeed != TargetSpeed) {
        CurrentSpeed = TargetSpeed;
        lastMovement = now;
      }
      break;
    case Stopping:
      if(elapsed >= StopTime) {
        state = Stopped;
      }
      break;
  }
/*
  // Skip if current direction and speed matches target
  if(CurrentSpeed == TargetSpeed && CurrentDirection == TargetDirection && elapsed < TargetDuration + SpinTime) return;

  if(CurrentSpeed == 0 && TargetSpeed != 0) {
    // Rover is stopping
    if(elapsed >= StopTime) {
      // Rover stopped -> spin up
      lastMovement = now;
      CurrentSpeed = SpinSpeed;
      CurrentDirection = TargetDirection;
    }
  } else if(CurrentDirection == TargetDirection) {
    // Rover is moving 
    if(CurrentSpeed == TargetSpeed && elapsed >= TargetDuration + SpinTime) {
      // Moved enough
      CurrentSpeed = 0;
      TargetSpeed = 0;
    } else if(elapsed >= SpinTime) {
      // Is spinned up
      CurrentSpeed = TargetSpeed;
    }
  } else if(CurrentDirection != TargetDirection) {
    // Stop
    lastMovement = now;
    CurrentSpeed = 0;
  }*/

  u8 power = map(CurrentSpeed, 0, 100, 0, 255);
  /*Teleplot.sendInt("Direction", direction);
  Teleplot.sendInt("Power", power);
  Teleplot.sendInt("Speed", speed);*/

  switch (CurrentDirection)
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
