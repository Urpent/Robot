#include "Robot.h"

//****
//  stateReady -> stateMoving -> stateStopping -> stateReady
//
//  state is in charge of executing logic
//
//****



void stateReady();
void stateMoving();
void stateStopping();
void stateCalibration();
void stateRightWallCalibration();
float minNum(float num1, float num2, float num3);
bool isCorrectWallDistance();

char calibrateDirection;
int calibCount = 0;
int calibCountRightWall = 0;
bool isTurning = false;

unsigned long urmTimer = 0;

boolean isBack = false;
boolean isCalibrationDone = true;
boolean isRightCaliDone = true;

void (*robotState)(void) = &stateReady;


//*****************************
//***********STATES************
//*****************************
void robot() {
  (*robotState)(); //Activate current state
}


void stateReady() {
  //Can CONSTANTLY read sensor value.
  // Serial.println("ready");
}

void stateMoving() {

  if (IsRun() && isCalibrationDone && isRightCaliDone && !isTurning) { //Stop robot if obstacle is too near
    if ( getSensorFrontRight() < 10 || getSensorFrontLeft() < 10 || getSensorFrontMid() < 10)
      robotStop();
  }
  //Serial.println("moving");
  if (!IsRun())
  {
    isRightCaliDone = false; //calibrated after moving
    robotState = &stateStopping;
  }
  // Serial.println("moving");
}

void stateStopping() { //Used send sensor value ONCE to PC
  rpm_t rpmFeedback = getRpm();
  if (rpmFeedback.L == 0 && rpmFeedback.R == 0) //Robot not stopped yet!
  {
    IsRun(false);

    isStopping = false;
  }
  else
    return;

  if (!isCalibrationDone) { //Robot still Calibrating
    robotState = &stateCalibration;
    return;
  }
  else if (!isRightCaliDone) {
    robotState = &stateRightWallCalibration;
    return;
  }

  robotState = &stateReady;
  isTurning = false;
  sensorsToPC(); //Printing Sensors value on serial will tell PC sensor value & robot is READY for next command
  //Serial.println("stopping");
}

void stateRightWallCalibration() {
startCali:
  float degree = 0;
  float sideRight = getSensorSideRight();
  float sideRight2 = getSensorSideRight2();
  float mid = getSensorFrontMid();
  float right = getSensorFrontRight();
  float left = getSensorFrontLeft();
  float minDist;
  float sensorsDifference = abs(sideRight - sideRight2);
  minDist = minNum(left, right, mid);
  degree = (atan(abs(sideRight - sideRight2) / 19) * 57.3); // 180/3.141 = 57.3
  if (sensorsDifference > 5 || sideRight  > 15 || sideRight2 > 15)
  {
    goto exitCalib1;
  }

  if (sideRight > sideRight2 && degree >= 1.5 && calibCountRightWall < 5) //sideRight sensor behind
  {
    calibCountRightWall++;
    //degree = (atan(abs(sideRight - sideRight2) / 19.5) * 57.3); // 180/3.141 = 57.3
    if (degree > 40) // recheck sensor if degree too high
      goto startCali;
    Serial.print("turned right ");
    Serial.println(degree);
    robotRight(degree, 50, 60); //robotRight(degree, speed) bigger degree rotate faster

  }
  else if (sideRight2 > sideRight && degree >= 1.5 && calibCountRightWall < 5) //sideRight2 sensor behind
  {
    calibCountRightWall++;
    //degree = (atan((sideRight2 - sideRight) / 19.5) * 57.3);
    if (degree > 40)
      goto startCali; // recheck sensor if degree too high

    Serial.print("turned left ");
    Serial.println(degree);
    robotLeft(degree, 50, 60); //17: distance from right to left sensor
  }
  else if (minDist - 10 >= 1 && minDist <= 13) { //too far from block
    //Serial.print("forward");
    //Serial.println(minDist);
    robotForward(minDist - 10, 30, 58);
  }
  else if (10 - minDist >= 1) {
    //Serial.print("back");
    //Serial.println(minDist);
    robotBack(10 - minDist, 30);
  }
  else {
exitCalib1:
    isRightCaliDone = true;
    robotState = &stateStopping;
    calibCountRightWall = 0;
  }
}

void stateCalibration() {
  float mid = getSensorFrontMid();
  float right = getSensorFrontRight();
  float left = getSensorFrontLeft();

  float sensorsDifference = abs(left - right);

  if (sensorsDifference > 6) {  //Changed from 15 to 7. (1 and 2 block difference still will rotate for 15).
    //TODO, any sensor too far cannot calibrate
    goto exitCalib;

  }

  float degree;
  float d;
  degree = (atan(abs(right - left) / 18.45) * 57.3);
  // d = abs(left - right);
  if (!isBack && mid <= 7) {
    robotBack(9 - mid, 30);
    isBack = true;
  }
  else if (left > right && degree > 1.5 && calibCount < 8) //left sensor behind
  {
    calibCount++;
    //degree = (atan((left - right) / 18.45) * 57.3); // 180/3.141 = 57.3
    robotRight(degree, 50, 60); //robotRight(degree, speed) bigger degree rotate faster
    //Serial.print("turned right ");
    //Serial.println(degree);
  }
  else if (right > left && degree > 1.5 && calibCount < 8) //right sensor behind
  {
    calibCount++;
    //degree = (atan((right - left) / 18.45) * 57.3);
    robotLeft(degree, 50, 60); //17: distance from right to left sensor
    //Serial.print("turned left");
    //Serial.println(degree);
  }

  else if (right - 10 >= 1 && right <= 15) { //too far from block
    robotForward(right - 10, 30, 65);
    Serial.print("forward ");
    Serial.println(right - 10);
  }

  else if (10 - right >= 1  ) { //too near to block
    robotBack(10 - right, 30);
    Serial.print("back ");
    Serial.println(10 - right);
  }
  else { //Exit
exitCalib:
    isCalibrationDone = true;
    switch (calibrateDirection) { //Need to turn back the robot.
      case 'f':
        robotState = &stateStopping;
        break;
      case 'l':
        robotRight(90, 82, 56);
        break;
      case 'r':
        robotLeft(90, 82, 56);
        break;
    }
    calibCount = 0; //Must reset to 0 if not next calib wont work.
    isBack = false;
  }

}
//*****************************
//************END**************
//*****************************


bool isRobotReady() {
  return robotState == &stateReady ? true : false;
}

void robotCalibration(char dir) {
  if (robotState == &stateReady) {

    isCalibrationDone = false;

    calibrateDirection = dir;
    switch (dir) {
      case 'f':
        robotState = &stateCalibration;
        break;
      case 'l':
        robotLeft(90, 82, 56);//TODO: need new ticks
        break;
      case 'r':
        if (!isCorrectWallDistance()) {
          robotRight(90, 82, 56);
        }
        else { //Don't calibrate
          isCalibrationDone = true;
          robotState = &stateStopping;
        }
        break;
    }
  }
}

bool isCorrectWallDistance() {
  bool ok = false;
  float sideRight = getSensorSideRight();
  float sideRight2 = getSensorSideRight2();
  if (abs(10 - sideRight) < 0.3 && abs(10 - sideRight2) < 0.3)
    ok = true;
  return ok;
}

void robotForward(int distanceCm, int speedRpm, int ticks) {
  if (robotState == &stateReady || robotState == &stateCalibration || robotState == &stateRightWallCalibration) {
    robotState = &stateMoving;
    motorForward(distanceCm, speedRpm, ticks);
  }
}

void robotBack(int distanceCm, int speedRpm) {
  if (robotState == &stateReady || robotState == &stateCalibration || robotState == &stateRightWallCalibration) {
    robotState = &stateMoving;
    motorBack(distanceCm, speedRpm);
  }
}

void robotLeft(int degree, int speedRpm, int ticks) {
  if (robotState == &stateReady || robotState == &stateCalibration || robotState == &stateRightWallCalibration) {
    robotState = &stateMoving;
    isTurning = true; //for right wall calibration
    motorLeft(degree, speedRpm, ticks);
  }
}

void robotRight(int degree, int speedRpm, int ticks) {
  if (robotState == &stateReady || robotState == &stateCalibration  || robotState == &stateRightWallCalibration) {
    robotState = &stateMoving;
    isTurning = true;//for right wall calibration
    motorRight(degree, speedRpm, ticks);
  }
}


//**********EXPLANATION*********
//robotStop() can be used directly from serial command
//OR
//by interrupt when distance is reached.
void robotStop() {
  if (robotState == &stateMoving) {
    motorStop();
    robotState = &stateStopping;
  }
}

void robotStatus() {
  Serial.print(F("L To Go: "));
  Serial.println(countToGoL);
  Serial.print(F("R To Go: "));
  Serial.println(countToGoR);
}
float minNum(float num1, float num2, float num3) {
  float result = num1;
  result = result > num2 ? num2 : result;
  result = result > num3 ? num3 : result;
  return result;
}

