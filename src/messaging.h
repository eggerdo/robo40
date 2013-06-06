#ifndef MESSAGING_H
#define MESSAGING_H

#include "aJSON.h"


#define HEADER 0xA5

#define SENSOR_DATA 0
#define DRIVE_COMMAND 1
#define MOTOR_COMMAND 2
#define CONTROL_COMMAND 3
#define DISCONNECT 4

#define INT_T 0
#define DOUBLE_T 1
#define STRING_T 2
#define BOOL_T 3

void setSerialLine(Stream *stream_);

int getType(aJsonObject* json);

aJsonObject* createJsonBase(int type);
aJsonObject* createSensorData();
aJsonObject* createSensorGroup(char* name);

void addSensorValue(aJsonObject *json, char* name, int value);
void addSensorValue(aJsonObject *json, char* name, double value);
void addSensorValue(aJsonObject *json, char* name, char* value);
void addSensorValue(aJsonObject *json, char* name, boolean value);

aJsonObject* createDriveCommand(int left, int right);
void decodeDriveCommand(aJsonObject *json, int* left, int* right);
aJsonObject* createMotorCommand(int motor_id, int direction, int speed);
void decodeMotorCommand(aJsonObject *json, int* motor_id, int* direction, int* speed);

// sendData also deletes the json object after it is sent
void sendData(aJsonObject *json);
aJsonObject* readData();

void deleteItem(aJsonObject* json);

#endif
