#ifndef Movement_h
#define Movement_h

#include <Arduino.h>
#include "DualVNH5019MotorShield.h"
#include "Robot.h"

//Global Variable
extern DualVNH5019MotorShield md;

extern volatile int countToGoL;
extern volatile int countToGoR;
extern int accelerateTimer; 
extern int rpmSetL, rpmSetR,rpmFinalL , rpmFinalR,  rpmPrevL, rpmPrevR;
extern int directionL, directionR;


typedef struct {
  float L;
  float R;
} rpm_t;


//Get Set
bool IsRun();
void IsRun(bool);


//Function Prototype
rpm_t getRpm();
void motorSetRPM(float, float);
void isr_encoder_L();
void isr_encoder_R();
void motorForward(float distanceCm, int speedRpm,int ticks);
void motorBack(float distanceCm, int speedRpm);
void motorRight(float degree, int speedRpm,int ticks);
void motorLeft(float degree, int speedRpm,int ticks);
void motorStop();
void motorTestRPM() ;

#endif
