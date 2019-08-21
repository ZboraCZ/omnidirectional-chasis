#include <Wire.h>
#include <Stepper.h>

#define SLAVE_ADDRESS 0x04

byte receivedNumber = 0; // Received original integer from RPi

//Const for converting 1 byte(max 255) back to 360 degrees sent from RPi
const double CONV_DEG = 1.4117647058823529411764; //MUST MATCH CONST ON RPi for valid nums.

// We are getting 2 values.
//1st byte is 3 bits of instruction primitive (go, turn, rotate, stop,...) and 5 bits of angle 
//2nd byte is another 3 bits of angle, 4 bits of number in centimeters, last is Security bit
int nthValue = 1;

//number of steps per revolution for my motors
const int STEPS_PER_REVOLUTION = 200;
const int m1DirPin = 2; const int m1StepPin = 3;
const int m2DirPin = 4; const int m2StepPin = 5;
const int m3DirPin = 6; const int m3StepPin = 7;
Stepper motor1 = Stepper(STEPS_PER_REVOLUTION, m1DirPin, m1StepPin);
Stepper motor2 = Stepper(STEPS_PER_REVOLUTION, m2DirPin, m2StepPin);
Stepper motor3 = Stepper(STEPS_PER_REVOLUTION, m3DirPin, m3StepPin);


// 1° = 0.01745329252 rad
const float DEG_TO_RAD_MULTIPLIER = 0.01745329252;

//Speed multiplier for all the motors (0: motors don't spin. 1: motors go the fastest possible)
float magnitude = 0.5; 

void setup() {
  
  pinMode(m1DirPin, OUTPUT); pinMode(m2DirPin, OUTPUT);pinMode(m3DirPin, OUTPUT); 
  pinMode(m1StepPin, OUTPUT); pinMode(m2StepPin, OUTPUT); pinMode(m3StepPin, OUTPUT);
  
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  
  Serial.println("Ready for communication!");
}

void loop() {
  delay(100);
}

// Process received bytes from RPi to rotate wheels
void receiveData(int byteCount) {
  Serial.println("-----Data Received-----");
  
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
  //OR bit algebra operation: connect 5 bits of first byte and 3 bits of second byte to 1 byte
  int middleNumber = firstHalfMiddleNumber | secondHalfMiddleNumber;
  middleNumber = round(middleNumber * CONV_DEG);
  Serial.println(middleNumber);
  //Now we have all parts we need to rock. Lets Move!

  //========================== ACTION WITH MOTORS ================================
  switch(instructionPrimitiveCode){
    case 7: //or '111' code for 'Go' instruction
      //TODO
      break;
  }
}

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
