#include "Main.h"
#include "Communication.h"
#include "Pid.h"
#include "Robot.h"
#include "Sensor.h"


Agenda scheduler;

void setup() {
  Serial.begin(115200); // Serial.setTimeout(1000);
  analogReference(INTERNAL);
  attachInterrupt(digitalPinToInterrupt(3) , isr_encoder_R, CHANGE);//actual is pin 5
  attachInterrupt(digitalPinToInterrupt(2) , isr_encoder_L, CHANGE);
  Serial.println("s0");

  md.init();


  scheduler.insert(motorPID, 50000); //Need fine tune sample rate
  scheduler.insert(readSerial, 100000); //0.2s speed for check command from PC
  scheduler.insert(robot, 50000);

  
  //scheduler.insert(sensorsDisplay,100000); //print analog
  //scheduler.insert(sensorsToPC, 300000);
  //scheduler.insert(sensorsTest, 200000);
 // scheduler.insert( motorTestRPM , 2000000);
  
}

void sensorsTest(){
  Serial.println(analogRead(A0));
  
}

// Execute Multi-Tasking. DO NOT edit below!
void loop() {
  scheduler.update();
};
