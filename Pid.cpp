#include "Pid.h"

const float k1L = 2;//1.2;
const float k2L = -0.9; //-0.9;
const float k3L = 0.0066;

const float k1R = 2.08;//1.25;
const float k2R = -0.9;//-0.94;

const float k3R = 0.0066;
bool isStopping = false;
float errorNowL;
float errorPrev1L;
float errorPrev2L;

float errorNowR;
float errorPrev1R;
float errorPrev2R;

float extraSpeedL, extraSpeedR;
int accelerateTimer;

void motorPID() {
  float rpmPIDL, rpmPIDR;
  rpm_t rpmFeedback = getRpm();

  errorNowL = rpmSetL != 0 ? rpmSetL - rpmFeedback.L  : 0;
  errorNowR = rpmSetR != 0 ? rpmSetR - rpmFeedback.R : 0;

  rpmPIDL =  rpmPrevL + k1L * errorNowL + k2L * errorPrev1L + k3L * errorPrev2L;
  rpmPIDR =  rpmPrevR + k1R * errorNowR + k2R * errorPrev1R + k3R * errorPrev2R;

  //Maximum PID value allowed
  rpmPIDL = rpmPIDL > 225 ? 225 : rpmPIDL;
  rpmPIDR = rpmPIDR > 225 ? 225 : rpmPIDR;

  if (IsRun()) { //Execute PID
    if (directionL * directionR == -1  || !isCalibrationDone || !isRightCaliDone) {
      motorSetRPM(rpmPIDL, rpmPIDR);
    }
    else if (countToGoL < 700 && rpmFeedback.L > 100) {
      if ((rpmFeedback.L - rpmFeedback.R) > 1) {
        md.setBrakes(100, 160);

        //Serial.println("L more. HardBrakeL");
      }
      else if ( (rpmFeedback.R - rpmFeedback.L ) > 1) {
        md.setBrakes(160, 100);
        //Serial.println("R more. HardBrakeR");
      }
      else
      {
        md.setBrakes(107, 100);
        //Serial.println("Same Stop Hardbrake");
      }
      isStopping = true;
     // rpmSetR = rpmSetL = rpmFeedback.L - 10;
    }
    else if (countToGoL < 250 && rpmFeedback.L > 80) {
      isStopping = true;
      if ((rpmFeedback.L - rpmFeedback.R) > 1) {
        md.setBrakes(50, 97);
       // Serial.println("L More. MediumBrakeL");
      }

      else if ( (rpmFeedback.R - rpmFeedback.L ) > 1) {
        md.setBrakes(115, 43);
        //Serial.println("R More. MediumBrakeR");
      }
      else
      {
        md.setBrakes(66, 53);
        //Serial.println("Same Stop. MediumBrake");
      }
     // rpmSetR = rpmSetL = rpmFeedback.L - 10;
    }
    else if (isStopping)
    {
      //Serial.println("isStopping!");
      if ((rpmFeedback.L - rpmFeedback.R) > 1) {
        md.setBrakes(50, 67);
        //Serial.println("L More. MediumBrakeL");
      }

      else if ( (rpmFeedback.R - rpmFeedback.L ) > 1) {
        md.setBrakes(70, 23);
        //Serial.println("R More. MediumBrakeR");
      }
      else
      {
        md.setBrakes(66, 50);
        //Serial.println("Same Stop. MediumBrake");
      }
    }
    else {
      //Serial.println("Pid Move!");
      if (directionL * directionR == 1 && (rpmFeedback.L - rpmFeedback.R)  > 1.0  ) {
        //Serial.println("L is faster. Acc R");
        rpmPIDR +=1.1;
        motorSetRPM(rpmPIDL , rpmPIDR /*(rpmFeedback.L - rpmFeedback.R)/3*/);
      }
      else if (  directionL * directionR == 1 && (rpmFeedback.R - rpmFeedback.L) > 1.0)
      {
        //Serial.println("R is Faster. Acc L");
        rpmPIDL +=0.8;
        motorSetRPM(rpmPIDL  /*(rpmFeedback.R - rpmFeedback.L)/3*/, rpmPIDR);
      }
      else
      {
        //Serial.println("Same Speed");
        if ( countToGoL < countToGoR)
          motorSetRPM(rpmPIDL, rpmPIDR + 0.5);
        else if ( countToGoL > countToGoR)
          motorSetRPM(rpmPIDL + 0.5, rpmPIDR);
        else
          motorSetRPM(rpmPIDL, rpmPIDR);
      }

      if ( (directionL * directionR) == 1 && accelerateTimer > 0 && (accelerateTimer % 2) == 0 && rpmSetR < 200)
      {
        rpmSetR = rpmSetL = rpmSetR + 10;
        motorSetRPM(rpmPIDL + 10, rpmPIDR + 10);
      }
    }
  }
  accelerateTimer++;
  rpmPrevL = rpmPIDL;
  errorPrev1L = errorNowL;
  errorPrev2L = errorPrev1L;

  rpmPrevR = rpmPIDR;
  errorPrev1R = errorNowR;
  errorPrev2R = errorPrev1R;
}

