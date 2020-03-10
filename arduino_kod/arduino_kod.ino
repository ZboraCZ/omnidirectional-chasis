#include <Wire.h>
#include <AccelStepper.h>

#define SLAVE_ADDRESS 0x04

// this is the acutal being read number from the I2C bus
byte receivedNumber = 0;

//Const for converting 1 byte(max 255) back to 360 degrees sent from RPi
const double CONV_DEGREES = 1.4117647058823529411764; //MUST MATCH CONST ON RPi for valid nums.
//Const for converting degrees to radians for math calculations of motor movements
const double CONV_DEG_TO_RAD = 0.01745329251;

// We are getting 2 values.
//1st byte is 3 bits of instruction primitive (go, turn, rotate, stop,...) and 5 bits of angle 
//2nd byte is another 3 bits of angle, 4 bits of number in centimeters, last is Security bit
int nthValue = 1;

//MOTORS AND WHEELS DECLARATIONS
//Speed multiplier for all the motors (0: motors don't spin. 1: motors go the fastest possible)
const double WHEELS_MAGNITUDE = 1; 
// Diameter of wheels in cm
const double WHEELS_DIAMETER = 5.5;
//Wheels circumference in cm (Obvod kola)
const double WHEELS_CIRCUMFERENCE = WHEELS_DIAMETER * PI;
//Max rotates per second for AccelSteper library
const int MAX_RPS = 200;
//number of steps per revolution for my motors
const int STEPS_PER_REVOLUTION = 200;

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define motorInterfaceType 1
const int M1_DIR_PIN = 2; const int M1_STEP_PIN = 3;
const int M2_DIR_PIN = 4; const int M2_STEP_PIN = 5;
const int M3_DIR_PIN = 6; const int M3_STEP_PIN = 7;

//##############################################################################
//####################### MOTOR DECLARATION SECTION ############################
//##############################################################################

AccelStepper motor1 = AccelStepper(motorInterfaceType, M1_STEP_PIN, M1_DIR_PIN);
AccelStepper motor2 = AccelStepper(motorInterfaceType, M2_STEP_PIN, M2_DIR_PIN);
AccelStepper motor3 = AccelStepper(motorInterfaceType, M3_STEP_PIN, M3_DIR_PIN);

//####################### END OF MOTOR DECLARATION SECTION ############################

// 1° = 0.01745329252 rad
const float DEG_TO_RAD_MULTIPLIER = 0.01745329252;

void setup() {
  
  pinMode(M1_DIR_PIN, OUTPUT); pinMode(M2_DIR_PIN, OUTPUT);pinMode(M3_DIR_PIN, OUTPUT); 
  pinMode(M1_STEP_PIN, OUTPUT); pinMode(M2_STEP_PIN, OUTPUT); pinMode(M3_STEP_PIN, OUTPUT);

  //Sets max steps per second to motors. If higher than 1000, Stepper gets inconsistent.
  motor1.setMaxSpeed(1000); 
  motor2.setMaxSpeed(1000); 
  motor3.setMaxSpeed(1000); 
  
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  
  Serial.println("Arduino set and ready to rock!");
}

void loop() {
}

// Process received bytes from RPi to rotate wheels
void receiveData(int byteCount) {
  Serial.println("-----Receiving data-----");
  
  int offsetIndexesOfBytes = byteCount/2; // each offset is for 1 byte... using this int in FOR loop to jump over them as they are starting nums.
  
  byte instructionPrimitiveCode;
  byte firstHalfMiddleNumber;
  byte secondHalfMiddleNumber;
  byte distance;
  byte secureBit;
  
  for(int i =0; i<offsetIndexesOfBytes; i++){ //hopping over the starting offset indexes
      receivedNumber = Wire.read();
    }
  nthValue = 1;
  while (Wire.available()) {
    receivedNumber = Wire.read();
    Serial.println(receivedNumber);
    Serial.println(decToBinary(receivedNumber));
    Serial.println("------------");
        
    switch( nthValue ) {
      //First byte arrived (3 bits: instruction primitive code; 5 bits: first half of middle number)
      case 1: 
        instructionPrimitiveCode = receivedNumber >> 5;
        firstHalfMiddleNumber = receivedNumber << 3;
        nthValue++;
        break; 

      //Second byte arrived (3 bits: second half of middle number; 4 bits: distance measurement [cm], 1 bit: secure bit)
      case 2: 
        secondHalfMiddleNumber = receivedNumber >> 5;
        distance = receivedNumber << 3; distance = distance >> 4;
        secureBit = bitRead(receivedNumber, 0); //2nd parameter is nth position of bit from right
        nthValue = 1;
        break;
    }
  }
  
  //OR bit algebra operation: connect last 5 bits of first byte and first 3 bits of second byte to 1 byte
  int middleNumber = firstHalfMiddleNumber | secondHalfMiddleNumber;
  middleNumber = round(middleNumber * CONV_DEGREES);
  //Now we have all parts we need to rock. Lets Move!
  
  //========================== ACTION WITH MOTORS ================================
  switch(instructionPrimitiveCode){
    case 7: //or '111' code for 'Go' instruction
      //Convert degrees to radians to use math functions correctly
      double receivedAngle = middleNumber * CONV_DEG_TO_RAD;
      double vectorX = cos(receivedAngle) * WHEELS_MAGNITUDE;
      double vectorY = sin(receivedAngle) * WHEELS_MAGNITUDE;

      // How many rotates does the wheel need to spin to get to finish.
      double rotatesOfWheelsOnDistance = distance / WHEELS_CIRCUMFERENCE;
      //Calculate RPS for each motor <0,1>
      double motor1Speed = (-1 * vectorX) * MAX_RPS;
      double motor2Speed = (0.5 * vectorX + sqrt(3)/2 * vectorY) * MAX_RPS;
      double motor3Speed = (0.5 * vectorX - sqrt(3)/2 * vectorY) * MAX_RPS;

      //Set speed by RPS for each motor
      motor1.setSpeed(motor1Speed); motor2.setSpeed(motor2Speed); motor3.setSpeed(motor3Speed);

      //TODO: How to determine, that we reached the finish distance
      
      // Set the current position of motors to 0:
      motor1.setCurrentPosition(0); motor1.setCurrentPosition(0); motor1.setCurrentPosition(0);

      // Run the motor forward at 200 steps/second until the motor reaches 400 steps (2 revolutions):
      while(motor1.currentPosition() != 1000)
      {
        // Setting 200 by default to find out if code works for all motors... Making 5 turns.
        motor1.setSpeed(MAX_RPS); motor1.setSpeed(MAX_RPS); motor1.setSpeed(MAX_RPS);
        motor1.runSpeed(); motor1.runSpeed(); motor1.runSpeed();
      }

  
      break; //<- case 7: "Go" primitive instruction END.
  } //<- switch(instructionPrimitiveCode) END.
} //<- void receiveData() END.

// function to convert decimal to binary 
String decToBinary(byte n) 
{ 
  String binar = "";
  while(n>0){
    binar = n%2 + binar;
    n = n/2;  
  }
  return binar;
} 


// Function to convert binary to decimal
int binaryToDecimal(String n)
{
  String num = n;
  int dec_value = 0;

  // Initializing base value to 1, i.e 2^0
  int base = 1;

  int len = num.length();
  for (int i = len - 1; i >= 0; i--) {
    if (num[i] == '1')
      dec_value += base;
    base = base * 2;
  }

  return dec_value;
}


//========================== Sending data back to RPi - not working ============================
void sendData() {
  //byte output[] = {0x01,0x02,0x03,0x04};  // This is just some sample data for testing
  byte output[] = {255};
  Wire.write(output, 4);
}
