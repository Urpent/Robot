#include "Sensor.h"

//***Function Prototype***
float getSensorCm(uint8_t sensorNum);
int getSensorAnalog(uint8_t sensorNum);
float getSensorCmQuick(uint8_t sensorNum);


float analogtoCm(uint8_t sensorNum, int analogValue);
void bubbleSort(int A[], int len);
int average(int A[] );
//Function Prototype End


const uint8_t sensorAnalogNum[6] = {A0 /*Long Side Left*/,
                                    A1 /*Short front left*/,
                                    A2 /*Short Mid*/,
                                    A3 /*Short front Right*/,
                                    A4 /*Short Side Right*/,
                                    A5 /*Short Side Right2*/
                                   };

float offset[6] = {0, 0, 0, 0, 0, 0};


void sensorsToPC() {
  for (int s = 0 ; s < 5; s++) {
    Serial.print(getSensorCm(s));
    Serial.print(F("_"));
  }
  Serial.println(getSensorCm(5)); //Printline with last value
}

float getSensorFrontLeft() {
  return getSensorCm(1);
}
float getSensorFrontMid() {
  return getSensorCm(2);
}
float getSensorFrontRight() {
  return getSensorCm(3);
}
float getSensorSideRight() {
  return getSensorCm(4);
}
float getSensorSideRight2() {
  return getSensorCm(5);
}


//sensorsDisplay() prints value in cm. Used for testing.
void sensorsDisplay() {
  //Serial.println(analogRead(A0));
  //Serial.println( getSensorCm(0));
  Serial.print(getSensorAnalog(0));
  //Serial.print("_");
  //Serial.print(getSensorAnalog(1));
  //Serial.print("_");
  //Serial.print(getSensorAnalog(2));
  //Serial.print("_");
  //Serial.print(getSensorAnalog(3));
  //Serial.print("_");
  //Serial.print(getSensorAnalog(4));
  //Serial.print("_");
  //Serial.print(getSensorAnalog(5));
  
  Serial.println();
  

  
}

//Get Sensor reading with average of multiple samples
int getSensorAnalog(uint8_t sensorNum) {
  int sensorValuesBuffer[12]; //Read Analog 12 times for each sensor

  for (int i = 0 ; i < 2 ; i++) // Dump first two value.
    analogRead( sensorAnalogNum[sensorNum]);

  for (int i = 0; i < 12 ; i++)
    sensorValuesBuffer[i] = analogRead( sensorAnalogNum[sensorNum]);

  bubbleSort(sensorValuesBuffer, 12); //Sort for mode

  return average(sensorValuesBuffer);
}
int getSensorAnalogQuick(uint8_t sensorNum) {
  int sensorValuesBuffer[3]; //Read Analog 12 times for each sensor
  analogRead( sensorAnalogNum[sensorNum]);//Dump first value.
  sensorValuesBuffer[0] = analogRead( sensorAnalogNum[sensorNum]);
  sensorValuesBuffer[1] = analogRead( sensorAnalogNum[sensorNum]);
  sensorValuesBuffer[2] = analogRead( sensorAnalogNum[sensorNum]);
  bubbleSort(sensorValuesBuffer, 3); //Sort for mode
  return sensorValuesBuffer[1];
}

float getSensorCmQuick(uint8_t sensorNum) {
  return analogtoCm(sensorNum, getSensorAnalogQuick(sensorNum) );
}

float getSensorCm(uint8_t sensorNum) {
  return analogtoCm(sensorNum, getSensorAnalog(sensorNum) );
}


float analogtoCm(uint8_t sensorNum, int analogValue) {
  float distance;

  if (analogValue == 0) //0 analog means very far
    return 999; //999cm

  switch (sensorNum) {
    case 0: //Long Side Left
      if (analogValue >= 916)
        distance = 10;
      else
        distance = 455380 * pow(analogValue, -1.49);//LONG LEFT, 455380x-1.49
      break;
    case 1: distance = 100806 * pow(analogValue, -1.388) ; // Front left, centimeters 100806x-1.388
      break;
    case 2: distance = 78373 * pow(analogValue, -1.345) ; // Front mid, 78373x-1.345
      break;
    case 3: distance = 179199 * pow(analogValue, -1.486) - offset[sensorNum]; //front right 179199x-1.486
      break;
    case 4: distance = 111156 * pow(analogValue, -1.382) - offset[sensorNum]; //Right 111156x-1.382
      break;
    case 5: distance = 101357 * pow(analogValue, -1.386) - offset[sensorNum]; //Right 101357x-1.386
      break;
    default: distance = 0;
  }
  return distance;
}




void initCalibration() {
  offset[3] = 0;
  offset[3] = getSensorCm(4) - 10;
  offset[4] = 0;
  offset[4] = getSensorCm(5) - 10;
}

//From Arduino lib
void bubbleSort(int A[], int len) {
  unsigned long newn;
  unsigned long n = len;
  float temp = 0.0;
  do {
    newn = 1;
    for (int p = 1; p < len; p++) {
      if (A[p - 1] > A[p]) {
        temp = A[p];         //swap places in array
        A[p] = A[p - 1];
        A[p - 1] = temp;
        newn = p;
      } //end if
    } //end for
    n = newn;
  } while (n > 1);
}

int average(int A[] ) { //average without first 2 and last two value
  int sum = 0;

  for (int i = 2; i <= 9 ; i++) {
    sum += A[i];
  }

  return sum / 8;
}
