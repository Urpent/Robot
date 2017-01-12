#include "Movement.h"

float tickPerCm = 60;

DualVNH5019MotorShield md;

volatile bool isRun;
volatile int countToGoL, countToGoR;

int rpmSetL, rpmSetR, rpmFinalL , rpmFinalR, rpmPrevL, rpmPrevR;
unsigned long timePrev;
unsigned int countPrevL, countPrevR;



volatile int EncoderCountL, EncoderCountR;

int directionL = 0, directionR = 0; //Rotate forward = 1, Rotate backward = -1, 0 = stop

unsigned int countNowL, countElapsedL, countNowR, countElapsedR;
unsigned long timeElapsed;

void motorSetRPM(float rpmL, float rpmR) {
  if (!isRun)
    return;

  int speedL = rpmL != 0 ? (float)rpmL * 1.7 - 43.7 : 0;
  int speedR = rpmR != 0 ? (float)rpmR * 1.7 - 42.6 : 0;

  //Serial.print(speedL);
  //Serial.print("_");
  //Serial.println(speedR);
  if (isRun)
    md.setSpeeds(speedR * directionR, speedL * directionL);
}

void isr_encoder_L() {
  EncoderCountL++;
  if (countToGoL <= 20 ) { //need to be low for small recalibration to work
    motorStop();//Important
  }
}

void isr_encoder_R() {
  EncoderCountR++;
  if ( countToGoR <= 20) {
    motorStop(); //Important
  }
}

rpm_t getRpm() {
  rpm_t rpmFeedback;
  unsigned long timeNow = micros();

  noInterrupts();   //Turn interrupts off to prevent data corruption due to race condition
  countNowL = EncoderCountL;        //Access the shared variable
  countNowR = EncoderCountR;        //Access the shared variable
  interrupts();
  countElapsedL = countNowL - countPrevL;
  countElapsedR = countNowR - countPrevR;

  countPrevL = countNowL;
  countPrevR = countNowR;

  if (countElapsedL == 0 && countElapsedR == 0) //If robot NOT moving then RPM is 0.
  {
    rpmFeedback.L = 0;
    rpmFeedback.R = 0;
  }
  else {
    timeElapsed = timeNow - timePrev;
    timePrev = timeNow;
    //2248.86___ 562.215
    rpmFeedback.L = 53360.36 / (timeElapsed / countElapsedL);
    rpmFeedback.R = 53360.36 / (timeElapsed / countElapsedR);
    //Serial.print(rpmFeedback.L );
    //Serial.print("_");
    //Serial.println( rpmFeedback.R );
  }
  noInterrupts();
  countToGoL -= countElapsedL;
  countToGoR -= countElapsedR;
  interrupts(); //Must turn interrupts back on so that we will not call noInterrupt() twice which may lead to nasty bug.
  return rpmFeedback;
}



void motorForward(float distanceCm, int speedRpm, int ticks) {
  isRun = true;//todo: shift to robot.cpp

  if (distanceCm >= 12) {
    ticks = 59;
  }

  countToGoL = countToGoR = ticks * distanceCm; //58 * cm;
  //rpmFinalL = rpmFinalR = speedRpm;
  rpmSetL = rpmSetR = speedRpm;

  rpmPrevL = rpmPrevR = 0;
  accelerateTimer = 0;
  directionL = directionR = 1;
  timePrev = micros(); //Important!

  motorSetRPM( rpmSetL, rpmSetR);
}

void motorRight(float degree, int speedRpm, int ticks) {
  isRun = true;
  directionL = 1;
  directionR = -1;

  //float cm = 13.7 * 2; //13.7 = 90degree, 0.1522.. = 1 degree  //13.35 = 90 degree
  float cm = 0.146  * degree;

  countToGoL = countToGoR = ticks * cm; // 69.4 *cm;//59.68 * cm;

  rpmSetL = rpmSetR = speedRpm; //TODO: speedRpm/2


  rpmPrevL = rpmPrevR = 0;

  timePrev = micros(); //Important!
  motorSetRPM( rpmSetL, rpmSetR);
}


void motorLeft(float degree, int speedRpm, int ticks) {
  isRun = true;
  directionL = -1;
  directionR = 1;

  //  float cm = 13.7 * 2; //13.7 = 90degree, 0.3044.. = 1 degree  //13.35 = 90 degree

  float cm = 0.146 * degree;

  countToGoL = countToGoR = ticks * cm;

  rpmSetL = rpmSetR = speedRpm;
  rpmPrevL = rpmPrevR = 0;

  timePrev = micros(); //Important!
  motorSetRPM( rpmSetL, rpmSetR);
}


void motorBack(float distanceCm, int speedRpm) {
  isRun = true;
  float ticks = 60;
  
  countToGoL =  countToGoR = ticks * distanceCm;
  countToGoR = ticks * distanceCm;

  rpmSetL = speedRpm;
  rpmSetR = speedRpm;
  rpmPrevL = 0;
  rpmPrevR = 0;

  directionL = -1;
  directionR = -1;
  timePrev = micros(); //Important!

  motorSetRPM(rpmSetL,  rpmSetR);
}


void motorStop() {
  isRun = false;

  //if (countToGoL < 0 || countToGoR < 0)
  //  md.setBrakes(400, 400);
  //else if (countToGoL < countToGoR )
  //  md.setBrakes(390, 400);
  //else if (countToGoL > countToGoR)
  //  md.setBrakes(400, 390);
  //else
    md.setBrakes(400, 400);


  rpmSetL = rpmSetR = 0;
}

bool IsRun() {
  return isRun;
}

void IsRun(bool b) {
  isRun = b;
}



int counter = 1;
void motorTestRPM() {
  //Serial.println(counter);
  int speedNew = counter * 50;
  if (speedNew <= 400)
  {
    Serial.println(speedNew);
    md.setSpeeds(speedNew, speedNew); //m1 , m2
    counter++;
  }
  else
    md.setBrakes(300, 300);
}
