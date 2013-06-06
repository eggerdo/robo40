#include "messaging.h"

aJsonStream *jsonStream;
Stream *serial_stream;

void setSerialLine(Stream *stream_) {
	serial_stream = stream_;
	jsonStream = new aJsonStream(stream_);
}

int getType(aJsonObject* json) {
	aJsonObject* header;
	aJsonObject* type;
	header = aJson.getObjectItem(json, "header");
	type = aJson.getObjectItem(header, "type");
	return type->valueint;
}

// aJsonObject *json, *header, *data;
aJsonObject* createJsonBase(int type) {
  aJsonObject *json = aJson.createObject();

  aJsonObject *header = aJson.createObject();
  aJson.addNumberToObject(header, "id", HEADER);
  aJson.addNumberToObject(header, "timestamp", 0);
  aJson.addNumberToObject(header, "type", type);
  aJson.addItemToObject(json, "header", header);

  return json;
}

aJsonObject* createSensorData() {
	aJsonObject* json = createJsonBase(SENSOR_DATA);

	aJsonObject *data = aJson.createArray();
	aJson.addItemToObject(json, "data", data);

	return json;
}

void addSensorValue(aJsonObject *json, char* name, int value) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* item = aJson.createObject();
	aJson.addStringToObject(item, "name", name);
	aJson.addNumberToObject(item, "type", INT_T);
	aJson.addNumberToObject(item, "value", value);
	aJson.addItemToArray(data, item);
}

void addSensorValue(aJsonObject *json, char* name, double value) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* item = aJson.createObject();
	aJson.addStringToObject(item, "name", name);
	aJson.addNumberToObject(item, "type", DOUBLE_T);
	aJson.addNumberToObject(item, "value", value);
	aJson.addItemToArray(data, item);
}

void addSensorValue(aJsonObject *json, char* name, char* value) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* item = aJson.createObject();
	aJson.addStringToObject(item, "name", name);
	aJson.addNumberToObject(item, "type", STRING_T);
	aJson.addStringToObject(item, "value", value);
	aJson.addItemToArray(data, item);
}

void addSensorValue(aJsonObject *json, char* name, boolean value) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* item = aJson.createObject();
	aJson.addStringToObject(item, "name", name);
	aJson.addNumberToObject(item, "type", BOOL_T);
	if (value) {
		aJson.addTrueToObject(item, "value");
	} else {
		aJson.addFalseToObject(item, "value");
	}
	aJson.addItemToArray(data, item);
}

aJsonObject* createDriveCommand(int left, int right) {
	aJsonObject* json = createJsonBase(DRIVE_COMMAND);

	aJsonObject* data = aJson.createObject();
	aJson.addNumberToObject(data, "left", left);
	aJson.addNumberToObject(data, "right", right);
	aJson.addItemToObject(json, "data", data);

	return json;
}

void decodeDriveCommand(aJsonObject* json, int* left, int* right) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* left_j = aJson.getObjectItem(data, "left");
	*left = left_j->valueint;

	aJsonObject* right_j = aJson.getObjectItem(data, "right");
	*right = right_j->valueint;
}

aJsonObject* createMotorCommand(int motor_id, int direction, int speed) {
	aJsonObject* json = createJsonBase(MOTOR_COMMAND);

	aJsonObject* data = aJson.createObject();
	aJson.addNumberToObject(data, "motor_id", motor_id);
	aJson.addNumberToObject(data, "direction", direction);
	aJson.addNumberToObject(data, "speed", speed);
	aJson.addItemToObject(json, "data", data);

	return json;
}

void decodeMotorCommand(aJsonObject* json, int* motor_id, int* direction, int* speed) {
	aJsonObject* data = aJson.getObjectItem(json, "data");

	aJsonObject* motor_id_j = aJson.getObjectItem(data, "motor_id");
	*motor_id = motor_id_j->valueint;

	aJsonObject* direction_j = aJson.getObjectItem(data, "direction");
	*direction = direction_j->valueint;

	aJsonObject* speed_j = aJson.getObjectItem(data, "speed");
	*speed = speed_j->valueint;
}

void sendData(aJsonObject* json) {
  aJson.print(json, jsonStream);
  serial_stream->write("\n");
  aJson.deleteItem(json);
}

void deleteItem(aJsonObject* json) {
	aJson.deleteItem(json);
}

aJsonObject* readData() {
	aJsonObject* json;
	if (jsonStream->available()) {
		json = aJson.parse(jsonStream);
		return json;
	}
	return NULL;
}