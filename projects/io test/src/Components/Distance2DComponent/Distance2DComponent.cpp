#include "Distance2DComponent.h"

/// 3ms timeout (0.5148 meters)
time DistanceTimeout = 6'000;

Distance2DHwComponent::Distance2DHwComponent(pin trigger, pin echoLeft, pin echoRight, u8 distance) {
  Trigger = getPinDefinition(trigger);
  EchoLeft = getPinDefinition(echoLeft);
  EchoRight = getPinDefinition(echoRight);
  Distance = distance;
}

void Distance2DHwComponent::init() {
  pinMode(Trigger.Pin, OUTPUT);
  pinMode(EchoLeft.Pin, INPUT);
  pinMode(EchoRight.Pin, INPUT);
  digitalWrite(Trigger.Pin, LOW);
}

void Distance2DHwComponent::stop() {
  fastDigitalWrite(Trigger, LOW);
  Emitting = false;
  Emit = 0;
}

bool Distance2DHwComponent::measure(Point &position) {
  bool vLeft = false;
  bool vRight = false;
  bool dLeft = false;
  bool dRight = false;
  Left = 0;
  Right = 0;
  

  // Toggle trigger for 10 us
  fastDigitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  fastDigitalWrite(Trigger, LOW);

  // Wait for old pulse to end
  do {
    vLeft = fastDigitalRead(EchoLeft) == HIGH;
    vRight = fastDigitalRead(EchoRight) == HIGH;
  } while(vLeft || vRight);

  // Measure pulse from sensors
  MeasureStarted = micros();
  do {
    vLeft = fastDigitalRead(EchoLeft) == HIGH;
    vRight = fastDigitalRead(EchoRight) == HIGH;
    MeasureEnded = micros();

    if(!Left && vLeft) Left = MeasureEnded;
    else if(Left && !vLeft) {
      Left = MeasureEnded - Left;
      dLeft = true;
    }
    
    if(!Right && vRight) Right = MeasureEnded;
    else if(Right && !vRight) {
      Right = MeasureEnded - Right;
      dRight = true;
    }

  } while(MeasureEnded - MeasureStarted < DistanceTimeout && (!dLeft || !dRight));
  
  if(!dLeft || !dRight) return false;// || Left > DistanceTimeout || Right > DistanceTimeout) return false;

  // Signal travels to obstacle and back
  double distance_left = SPEED_OF_SOUND_MM_US * Left / 2.0;
  double distance_right = SPEED_OF_SOUND_MM_US * Right / 2.0;
  
  // Cicles intersection
  // https://planetcalc.com/8098/
  double a = (pow(distance_left, 2) - pow(distance_right, 2) + pow(Distance, 2)) / (2.0*Distance);
  double h = sqrt(pow(distance_left, 2) - pow(a, 2));

  position.X = (int)round(a);
  position.Y = (int)round(h);
  
  return true;
}