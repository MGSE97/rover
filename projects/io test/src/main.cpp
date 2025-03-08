#include <Arduino.h>
#include <HwComponent.hpp>
#include <Laser.hpp>
#include "display.hpp"
#include <LightSensor.hpp>
#include <Motor.hpp>

PoweredOnDisplay Display(10, U8G_I2C_OPT_FAST);
LaserHwComponent Laser(PIN_PB4);
LightSensorHwComponent LightSensor1(PIN_A0);
LightSensorHwComponent LightSensor2(PIN_A1);
MotorHwComponent Motor(PIN_PB3, PIN_PB2);

const uint8_t ComponentsLen = 5;
HwComponent* Components[ComponentsLen] = {
  &Laser,
  &Display,
  &LightSensor1,
  &LightSensor2,
  &Motor,
};

// put your setup code here, to run once
void setup() {
  // Serial monitor output stup
  Serial.begin(230400);
  Serial.println("Serial monitor connected.");
  
  for(u8 i = 0; i < ComponentsLen; i++) {
    Components[i]->init();
  }
  Serial.flush();
}

long int lastDraw = 0;

void loop() {
  int16_t a = LightSensor1.receive();
  int16_t b = LightSensor2.receive();
  
  Motor.drive(a > 500 || b > 500);

  Teleplot.send("LS1", a);
  Teleplot.send("LS2", b);
  Serial.flush();

  if(millis() - lastDraw > 1000) {
    Laser.emitToggle();
    
    lastDraw = millis(); 
 }

  if(Display.shouldDraw()) {
    Display.draw();
  }
}