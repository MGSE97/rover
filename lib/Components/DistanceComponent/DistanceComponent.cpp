#include "DistanceComponent.h"

DistanceHwComponent::DistanceHwComponent(pin trigger, pin echo) {
  Trigger = getPinDefinition(trigger);
  Echo = echo;
}

void DistanceHwComponent::init() {
  pinMode(Trigger.Pin, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger.Pin, LOW);
  
}

void DistanceHwComponent::stop() {
  fastDigitalWrite(Trigger, LOW);
  Emitting = false;
  Emit = 0;
}

bool DistanceHwComponent::measure(double& distance_cm, time& duration_us) {
  // Toggle trigger each 10 us
  fastDigitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  fastDigitalWrite(Trigger, LOW);

  // Measure pulse from sensor
  duration_us = pulseIn(Echo, HIGH, 5'000);
  
  // Signal travels to obstacle and back
  distance_cm = SPEED_OF_SOUND_MM_US * duration_us / 2.0;
  
  return duration_us > 0;
}