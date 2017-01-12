#include "FastestPath.h"

const byte b_size = 80;
const byte c_size = 20;

int startFastestPathID = 0;


char inputCommand[b_size];
char *command[c_size];
byte commandIndex = 0;

void storeFastestPath(char *commands) { // need to spilt command into substring, commands contain string such as: f_10,r_90,f_20

  strncpy(inputCommand, commands, 79); //Make a copy to another buffer
  inputCommand[b_size - 1] = '\0';

  int i = 0;
  for (char *p = strtok(inputCommand, ","); p != NULL; p = strtok(NULL, " ,"))
  {
    command[i++] = p;
  }
  commandIndex = 0;
}


void startFastestPath() {
  if (*command[commandIndex] == 'x') {
    scheduler.remove(startFastestPathID);
    return;
  }

  if (isRobotReady())
  {
    char *input[2];
    byte i = 0;
    for (char *p = strtok(command[commandIndex], "_"); p != NULL; p = strtok(NULL, " _"))
    {
      input[i++] = p;
    }
    int val = strtol(input[1], NULL, 10);

    switch ((char)*input[0]) {
      case 'f': //Move Forward
        robotForward(val, 100, 59); //robotForward(cm, speed)
        break;
      case 'l': //Move Left
        robotLeft(90, 82,56); //default
        break;
      case 'r':
        robotRight(90, 82,56); //TODO: need new ticks
        break;
      case 'b':
        robotBack(val, 60);
        break;
    }

    commandIndex++;
  }

}


