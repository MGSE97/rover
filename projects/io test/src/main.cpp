#include <LaserComponent.h>
#include <LightSensorComponent.h>
#include <SwitchesComponent.h>
#include "info_display.h"


InfoDisplay Display(10, U8G_I2C_OPT_FAST);
long int lastSceneChange = 0;

LaserHwComponent Laser(PIN_PB4);
long int emitChange = 0;

LightSensorHwComponent LightSensor1(PIN_A0);
LightSensorHwComponent LightSensor2(PIN_A1);
LightSensorsData lightSensorsData;

MotorDriverHwComponent MotorDriver({PIN_PB3, PIN_PB2, PIN_PD5}, {PIN_PB1, PIN_PB0, PIN_PD6}, 100, 50);
MotorDriverData motorDriverData={Stop,50};
long int driveChange = 0;
bool increaseSpeed = true;

uint8_t SwitchesPins[6] = {PIN_PD2, PIN_PD3, PIN_PD4, PIN_PD7, PIN_A2, PIN_A3};
SwitchesHwComponent Switches(SwitchesPins);
SwitchesData switchData = {{false,false,false,false,false,false}};

const uint8_t ComponentsLen = 6;
HwComponent* Components[ComponentsLen] = {
  &Laser,
  &Display,
  &LightSensor1,
  &LightSensor2,
  &MotorDriver,
  &Switches,
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

void loop() {
  long int now = millis();
  Switches.update();

  uint16_t a = LightSensor1.receive();
  uint16_t b = LightSensor2.receive();

  Teleplot.sendInt("LS1", a);
  Teleplot.sendInt("LS2", b);

  if(now - emitChange > 500) {
    if(Switches[0].State) {
      Laser.emitToggle();
    } else {
      Laser.emit(LOW);
    }
    emitChange = now; 
  }

  if(now - driveChange > 20) {
    if(Switches[1].State) {
      if(motorDriverData.speed == 0) {
        switch(motorDriverData.direction) {
          case Stop:
            motorDriverData.direction = Left;
            break;
          case Left:
            motorDriverData.direction = Right;
            break;
          case Right:
            motorDriverData.direction = Stop;
            break;
          default:
            break;
        }
        increaseSpeed = true;
      }
      
      if(increaseSpeed) motorDriverData.speed+=2;
      else              motorDriverData.speed-=2;

      if(motorDriverData.speed > 100) increaseSpeed = false;
      
      MotorDriver.drive(motorDriverData.direction, motorDriverData.speed);
    } else {
      MotorDriver.drive(Stop, 0);
    }
    driveChange = now;
  }

  if(Display.shouldDraw()) {
    bool canSceneChange = now - lastSceneChange > 2000;
    Teleplot.sendInt("LSC", now - lastSceneChange);
    Teleplot.sendInt("CSC", canSceneChange);

    if(canSceneChange) {
      if(Switches.Changed) {
        Display.Scene = SwitchesStatus;
        Switches.Changed = false;
        lastSceneChange = now;
      }
      else if(a > 500 || b > 500) {
        Display.Scene = LightSensorStatus;
        lastSceneChange = now;
      } 
      else if(motorDriverData.direction != Stop) {
        Display.Scene = MotorDriverStatus;
        lastSceneChange = now;
      }
      else {
        Display.Scene = PoweredOnStats;
      }
    }
    
    switch (Display.Scene)
    {
      case LightSensorStatus:  
        lightSensorsData = {a, b};
        Display.LightSensors = &lightSensorsData;
        break;
      case MotorDriverStatus:
        Display.MotorDriver = &motorDriverData;
        break;
      case SwitchesStatus:
        for(uint8_t i = 0; i < 6; i++){
          switchData.values[i] = Switches[i].State;
        }
        Display.Switch = &switchData;
        break;
      default:
        break;
    }

    Display.draw();
  }
  Serial.flush();
}