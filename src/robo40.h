// Peet's test
#include "aJSON.h"

#define MAXINCIDENTCOUNT  100

// Pin assignments


#define E1  8    // motor 1
#define E2  9
#define M1  52

#define S1  A0
#define S2  A1
  
#define E3  10   // motor 2
#define E4  11
#define M2  53
 
#define E7  4   // motor 2
#define E8  5
#define M4  49
 
#define E9  2   // motor 2
#define E10 3
#define M5  48

#define S3  A2
#define S4  A3

#define E5  6   // hulpmotor 1
#define M3  51
#define E6  7   // hulpmotor 2
#define M4  50

#define CURRENTLIMIT 1000
#define WAIT1        5000


void setup();
void loop();
void handleBluetooth();
void sendSensorData();
void handleControlCommand(aJsonObject* json);
void handleDisconnect(aJsonObject* json);
void handleMotorCommand(aJsonObject* json);
void handleDriveCommand(aJsonObject* json);
void postDelay(unsigned long int* time, int delay);
void dostop();
void drive(int leftSpeed, int rightSpeed);
void secdrive(int value,int motor);
int capSpeed(int value);
