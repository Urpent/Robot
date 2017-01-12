
#include "Communication.h"

void menu();

void (*state)(void) = &menu;

char inData[50];
byte index = 0;


void readSerial() {
  while (Serial.available() > 0  /* && Robot Ready? */) {
    char received = Serial.read();
    inData[index++] = received;

    if (received == '\r') //**Remember to set carriage return on Arduino Serial Monitor
    {
      inData[index] = '\0'; //Always Null-Terminate the string
      (*state)(); //Activate current state
      index = 0;
    }
  }
}

void menu() {
  char *input[4]; //Eg. f:10 or r:90
  byte i = 0;
  for (char *p = strtok(inData, ":"); p != NULL; p = strtok(NULL, " :"))
  {
    input[i++] = p;
  }
  int rpiTest;
  int val, val1 , val2, val3;

  char * pEnd;
  if (input[0] != 'z') {
    val1 = strtol(input[1],NULL , 10);
    val2 = strtol(input[2], NULL, 10);
    val3 = strtol(input[3], NULL, 10);
   
  }


  switch ((char)*input[0]) {
  case '1':
    robotForward(1,80,60);
    break;
    case 'f': //Move Forward
      robotForward(val1, val2, val3); //robotForward(cm, speed)   // cm, speed, ticks
      break;
    case 'l': //Move Left
      robotLeft(val1, val2, val3); //default
      break;
    case 'r':
      robotRight(val1, val2, val3);
      break;
    case 'b':
      robotBack(10, 60);
      break;
    case 'p': //turn around
      robotLeft(val1, val2, val3); //degree, speed, ticks
      break;
    case 'h':  //Hold or Stop the robot
      robotStop();
      break;
    case 'c': //Calibration front
      robotCalibration('f');
      break;
    case 'd': //Calibration
      robotCalibration('r');
      break;
    case 'a': //Calibration
      robotCalibration('l');
      break;
    case 'n':  //Ask for Navigation Infomation (Sensors)

      sensorsToPC();
      break;
    case 's':  //Ask for Status
     
   
      robotStatus();
      //TODO: print robot status, whether it is ready or what.
      break;
    case 'q':
      break;
    case 'z':
      storeFastestPath(input[1]);
      break;
    case 'y':
      startFastestPathID = scheduler.insert(startFastestPath, 100000);
      break;
    case 't':
      rpiTest = val + 1;
      Serial.println(rpiTest);
      break;
    case 'i':
      initCalibration();
      break;
    default:
      Serial.println(F("Invalid Command"));
      //Serial.println(F("***MENU***\nc : config \ns : start"));
      //Need ask for resend of command. sometime receive weird character ð
  }

}











