#include "Arduino.h"
#include "robo40.h"
#include "messaging.h"
// #include "CircularBuffer.h"

unsigned long int nextBluetoothUpdateTime = 0;
unsigned long int timeNow = 0;

int CSense1 = 0;
int CSense2 = 0;
int ReportC1 = 0;
int ReportC2 = 0;

int curSpeedMotor1 = 0;
int curSpeedMotor2 = 0;
int curSpeedMotor3 = 0;

int notagain = 0;

/*
#define MAX_BUFFER 128
CircularBuffer<char, MAX_BUFFER> logbuffer;
boolean sendOutBuffer = false;


#define E1  8
#define M1  9
#define E2  10                        
#define M2  11                         
*/

int curleftSpeed = 0;
int currightSpeed = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("session started");

    // Serial1.begin(115200);
    Serial1.begin(57600);
	  setSerialLine(&Serial1);
    // Serial1.flush();	
    // Serial1.setTimeout(10);
    Serial.println("connected to BT module");

    //motors
    pinMode(M1, OUTPUT);  
    pinMode(M2, OUTPUT);
    
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
    pinMode(E3, OUTPUT);
    pinMode(E4, OUTPUT);

    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);

    Serial.println("entering main loop");
}

void loop()
{
    timeNow = millis();
    handleBluetooth();
}

aJsonStream ss(&Serial);

void handleBluetooth()
{

	// if (nextBluetoothUpdateTime <= timeNow)
	// {
		aJsonObject* item;
		int type;
		item = readData();
		if (item != NULL) {
      aJson.print(item, &ss);
      Serial.println(" ");

			type = getType(item);

			switch(type) {
				case DRIVE_COMMAND:
					handleDriveCommand(item);
					break;
				case MOTOR_COMMAND:
					handleMotorCommand(item);
					break;
				case CONTROL_COMMAND:
					handleControlCommand(item);
					break;
				case DISCONNECT:
					handleDisconnect(item);
					break;
			}
			deleteItem(item);
		}

	// 	postDelay(&nextBluetoothUpdateTime, BLUETOOTH_INTERVAL);

	// }
}

// TODO: this is a demonstration of how to send sensor data
// the number of sensor values per message is not defined
// you can make 1 call to addSensorValue or 100
// but right now only 4 sensor values are displayed on the android app
void sendSensorData() {
	aJsonObject* sensorData;
	sensorData = createSensorData();
	// the second parameter is the name of the sensor, any string is fine
	// the third parameter is the value, which can be either of type integer,
	//   double/float, string, or boolean
	addSensorValue(sensorData, "Motor1", CSense1);
	addSensorValue(sensorData, "Motor2", CSense2);
	addSensorValue(sensorData, "Peak Motor1",ReportC1);
	addSensorValue(sensorData, "Peak Motor2", ReportC2);
	sendData(sensorData);
}

void handleControlCommand(aJsonObject* json) {
	// is not necessary for now
}

void handleDisconnect(aJsonObject* json) {
	// is sent when the phone disconnects from the robot
	// best to turn off all motors here
}

void handleMotorCommand(aJsonObject* json) {
  Serial.println("1");
	int id = -1, direction = -1, value = -1;
	decodeMotorCommand(json, &id, &direction, &value);
  secdrive(value, id);
  Serial.println("2");
	// TODO: add here the function call to drive the brush motor
	//  id is in case we want to control other mothers, currently a value of 1 is sent
	//  direction, either 0 or 1, where 1 is "forward"
	//  speed, a value between 0 and 255
}

void handleDriveCommand(aJsonObject* json) {
	int leftSpeed = 0, rightSpeed = 0;
	decodeDriveCommand(json, &leftSpeed, &rightSpeed);
	drive(leftSpeed, rightSpeed);
}

void postDelay(unsigned long int* time, int delay)
{
	*time = timeNow + delay;
}

void dostop() {
	//  LOGd("\tmotor stop");
	drive(0,0);
}

void drive(int leftSpeed, int rightSpeed)
{
        int count = 0;
        int incidentcount = 0;
        
        int motor1pin = E1;
        int motor2pin = E3;

	int left = capSpeed(leftSpeed);
	int right = capSpeed(rightSpeed);

        CSense1 = 0;
        CSense2 = 0;
        ReportC1 = 0;
        ReportC2 = 0;       

        digitalWrite(M1,HIGH);  
        digitalWrite(M2,HIGH);

    Serial.println("in Drive");
      analogWrite(E1, 0);   //PWM Speed Control
      analogWrite(E2, 0);   //PWM Speed Control
      analogWrite(E3, 0);   //PWM Speed Control
      analogWrite(E4, 0);   //PWM Speed Control
    Serial.println("speed set to zero");

	//setDirection(&left, &right);
    if (leftSpeed > 0 && rightSpeed > 0)
    { // forward direction
    /*
        digitalWrite(M1,HIGH);  
        digitalWrite(M2,HIGH);
    */  
      motor1pin = E1;
      motor2pin = E3;    
    } 
    else if (leftSpeed < 0 && rightSpeed < 0)
    {  // backward
    /*
        digitalWrite(M1,LOW);  
        digitalWrite(M2,LOW);     
    */ 
      motor1pin = E2;
      motor2pin = E4;    
    }
    else if (leftSpeed < 0 && rightSpeed > 0)
    { // turning right
    /*
        digitalWrite(M1,HIGH);  
        digitalWrite(M2,LOW); 
    */     
      motor1pin = E2;
      motor2pin = E3;    
    }
    else  // only option left: if (leftSpeed > 0 && rightSpeed < 0)
    { // turning left
    /*
        digitalWrite(M1,LOW);  
        digitalWrite(M2,HIGH);
    */      
      motor1pin = E1;
      motor2pin = E4;    
    }

    while ((incidentcount < MAXINCIDENTCOUNT) && (curleftSpeed != abs(capSpeed(leftSpeed))) && (currightSpeed != abs(capSpeed(rightSpeed)))) {
      CSense1 = analogRead(A0);
      CSense2 = analogRead(A2);
      ReportC1 = (CSense1 > ReportC1 ? CSense1 : ReportC1); // track maximum current sensed
      ReportC2 = (CSense2 > ReportC2 ? CSense2 : ReportC2);
      //Serial.print("CSense1 :");Serial.println(CSense1);
      //Serial.print("CSense2 :");Serial.println(CSense2);
      if ( (CSense1 < CURRENTLIMIT) &&  (CSense2 < CURRENTLIMIT) ) { // check for current
        count++;  // if ok increase speed
      }
      else {
        count = count--; // if not ok decrease
        incidentcount++;
        if (incidentcount > MAXINCIDENTCOUNT) {
          count = 0; // reset speed
        }
      }
      // Serial.print("Count :");Serial.println(count);

      if (count > abs(capSpeed(leftSpeed)))
        left = abs(capSpeed(leftSpeed));
      else
        left = count;

      if (count > abs(capSpeed(rightSpeed)))
        right = abs(capSpeed(rightSpeed));
      else
        right = count;
        
      analogWrite(motor1pin, left);   //PWM Speed Control
      analogWrite(motor2pin, right);   //PWM Speed Control

      curleftSpeed = left;
      currightSpeed = right;
      //Serial.print("curleftSpeed :");Serial.println(curleftSpeed);
      //Serial.print("currightSpeed :");Serial.println(currightSpeed);
      delay(10);

      // sendSensorData();
    }
/*
    analogWrite(E1, abs(max(0, left)));   //PWM Speed Control
    analogWrite(M1, abs(min(0, left)));   //PWM Speed Control

    analogWrite(E2, abs(max(0, right)));   //PWM Speed Control
    analogWrite(M2, abs(min(0, right)));   //PWM Speed Control
*/

  Serial.println(left);
  Serial.println(right);
  Serial.println(ReportC1);
  Serial.println(ReportC2);

  sendSensorData();
}

void secdrive(int value,int motor)
{
    int count = 0;

    int motorPin;
    int motorPin1;
    int motorPin2;
    int enablePin;

    int curSpeed = 0;

    if (motor == 1) {
      motorPin1 = E5;
      motorPin2 = E6;
      enablePin = M3;
      curSpeed = curSpeedMotor1;
    } else if (motor == 2) {
      motorPin1 = E7;
      motorPin2 = E8;
      enablePin = M4;
      curSpeed = curSpeedMotor2;
    } else if (motor == 3) {
      motorPin1 = E9;
      motorPin2 = E10;
      enablePin = M5;
      curSpeed = curSpeedMotor3;
    }
    
    digitalWrite(enablePin,HIGH); // enable selected motor 

    //Serial.println("in Drive");

    //analogWrite(motorPin1, 0);   //PWM Speed Control set to zero
    //analogWrite(motorPin2, 0);   //PWM Speed Control

    //Serial.println("speed set to zero");

	//setDirection(&left, &right);
    if (curSpeed > 0)
    { // forward direction
      motorPin = motorPin1;
    /*
        digitalWrite(M1,HIGH);  
        digitalWrite(M2,HIGH);
    */  
    } 
    else
    {  // backward
      motorPin = motorPin2;
    /*
        digitalWrite(M1,LOW);  
        digitalWrite(M2,LOW);     
    */ 
    }

    while ((curSpeed != value)) {

      if (curSpeed < value)
        curSpeed++;  //  increase speed
      else
        curSpeed--;  //  decrease spedd
        // Serial.print("Count :");Serial.println(count);
      if (curSpeed == 0) {
        if (motorPin == motorPin1)
          motorPin = motorPin2;
        else
          motorPin = motorPin1;
      }
      
      // curSpeed = count;
        
      analogWrite(motorPin, curSpeed);   //PWM Speed Control
      delay(5);
    }

    // report motor speed to global
    if (motor == 1) {
      curSpeedMotor1 = curSpeed;
    } else if (motor == 2) {
      curSpeedMotor2 = curSpeed;
    } else if (motor == 3) {
      curSpeedMotor3 = curSpeed;
    }
}

int capSpeed(int value)
{
	return max(min(value,249),-249);   //half speed is enforced here
}
