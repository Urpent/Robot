#ifndef FastestPath
#define FastestPath

#include <Arduino.h>
#include "Main.h"
#include "Robot.h"

extern int startFastestPathID;

void storeFastestPath(char *commands);
void startFastestPath();
#endif

