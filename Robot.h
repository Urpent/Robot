#ifndef Robot_h
#define Robot_h

#include "Movement.h"
#include "Sensor.h"
#include "Pid.h"


extern bool isCalibrationDone;
extern bool isRightCaliDone;

bool isRobotReady();

void robotForward(int distanceCm, int speedRpm = 70, int ticks = 58); //Move how far & how fast.
void robotBack(int distanceCm, int speedRpm = 50);
void robotLeft(int degree, int speedRpm = 50, int ticks = 60);
void robotRight(int degree, int speedRpm = 50, int ticks = 60);
void robotStop();

void robotCalibration(char dir);


void robotStatus();

void robot(); //statemachine loop trigger by scheduler.

#endif
