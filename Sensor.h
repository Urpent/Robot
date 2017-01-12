#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <SharpIR.h>

//***Public Functions***
void sensorsToPC(); //Used to send PC. Currently sending Analog value. Considering sending cm so that we can see on other places.


float getSensorFrontRight();
float getSensorFrontLeft();
float getSensorFrontMid();

float getSensorSideRight();
float getSensorSideRight2();

void initCalibration();
void sensorsDisplay(); //For OWN Uses
//Public Funtioncs end

#endif
