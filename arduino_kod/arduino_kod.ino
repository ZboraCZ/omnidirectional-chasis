#include <Wire.h>
#include <AccelStepper.h>

#define SLAVE_ADDRESS 0x04

// this is the acutal being read number from the I2C bus
byte receivedNumber = 0;

byte movementCompleted = 0;

//Const for converting 1 byte(max 255) back to 360 degrees sent from RPi
const double CONV_DEGREES = 1.4117647058823529411764; //MUST MATCH CONST ON RPi for valid nums.
//Const for converting degrees to radians for math calculations of motor movements
const double CONV_DEG_TO_RAD = 0.01745329251;

// We are getting 2 values.
//1st byte is 3 bits of instruction primitive (go, turn, rotate, stop,...) and 5 bits of angle 
//2nd byte is another 3 bits of angle, 4 bits of number in centimeters, last is Security bit
int nthValue = 1;

//MOTORS AND WHEELS DECLARATIONS 
// Diameter of wheels in cm
const double WHEELS_DIAMETER = 5.5;
//Wheels circumference in cm (Obvod kola)
const double WHEELS_CIRCUMFERENCE = WHEELS_DIAMETER * PI; //=17.27875
const int CHASIS_CIRCUMFERENCE = round(2 * PI * 20); // =126  20 is chasis radius ending at the center of wheel
const double CM_ON_ONE_DEGREE_CHASIS_ROTATION = 360/CHASIS_CIRCUMFERENCE;  //=2.857142857;
const double MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION = CM_ON_ONE_DEGREE_CHASIS_ROTATION/0.04799655442; //=59.5280826202   0.04799655442 is cm per 1 wheel step
//Max steps per second for AccelSteper library
const int MAX_SPS = 450;
//Steps multiplier for max. speed level 15 from RPi into max of 450 SPS
const int STEP_MULTIPLIER = 30;

//Modifier for all Motors speeds multiplications. 
//Goes to max 450, this is default, if "Go" doesnt go first to set it.
int speedModifier = 200;

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define motorInterfaceType 1
const int M1_DIR_PIN = 2; const int M1_STEP_PIN = 3;
const int M2_DIR_PIN = 4; const int M2_STEP_PIN = 5;
const int M3_DIR_PIN = 7; const int M3_STEP_PIN = 6;

//##############################################################################
//####################### MOTOR DECLARATION SECTION ############################
//##############################################################################

AccelStepper motor1 = AccelStepper(motorInterfaceType, M1_STEP_PIN, M1_DIR_PIN);
AccelStepper motor2 = AccelStepper(motorInterfaceType, M2_STEP_PIN, M2_DIR_PIN);
AccelStepper motor3 = AccelStepper(motorInterfaceType, M3_STEP_PIN, M3_DIR_PIN);

//####################### END OF MOTOR DECLARATION SECTION ############################

void setup() {
  
  //Setting enable pins to be able to turn of powering motors to not burn them down by staying still
  //Motors are enabled only when they are moving. Otherwise are disabled(powered off)
  motor1.setEnablePin(8);
  motor2.setEnablePin(12);
  motor3.setEnablePin(13); 
  disableMotorsPower();

  //Sets max steps per second to motors. If higher than 1000, Stepper gets inconsistent.
  motor1.setMaxSpeed(MAX_SPS); 
  motor2.setMaxSpeed(MAX_SPS); 
  motor3.setMaxSpeed(MAX_SPS); 

  ////////CONFIGURING I2C COMMUNICATION//////
  Serial.begin(9600); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  Serial.println("Arduino is set and ready to rock!");
}

void loop() {
}


    //######################################################
      //########## RECEIVING DATA FROM RPI #############
    //######################################################
    
// Process received bytes from RPi to move
void receiveData(int byteCount) {
  movementCompleted = 0;
  Serial.println("-----Receiving data-----");
  
  int offsetIndexesOfBytes = byteCount/2; // each offset is for 1 byte... using this int in FOR loop to jump over them as they are starting nums.
  
  byte instructionPrimitiveCode;
  byte firstHalfDegrees;
  byte secondHalfDegrees;
  byte speedLevel;
  byte secureBit;
  /* Debugging 
  for(int i =0; i<=byteCount; i++){ //hopping over the starting offset indexes
      receivedNumber = Wire.read();
      Serial.println(byteCount);
      Serial.println(receivedNumber);
      Serial.println("%%%%%%%%%%%");
    }
    */
  for(int i =0; i<offsetIndexesOfBytes; i++){ //hopping over the starting offset indexes
      receivedNumber = Wire.read();
    }
    
  nthValue = 1;
  while (Wire.available()) {
    receivedNumber = Wire.read();
            
    switch( nthValue ) {
      //First byte arrived (3 bits: instruction primitive code; 5 bits: first half of middle number)
      case 1: 
        instructionPrimitiveCode = receivedNumber >> 5;
        firstHalfDegrees = receivedNumber << 3;
        nthValue++;
        break; 

      //Second byte arrived (3 bits: second half of middle number; 4 bits: speedLevel measurement [cm], 1 bit: secure bit)
      case 2: 
        secondHalfDegrees = receivedNumber >> 5;
        speedLevel = receivedNumber << 3; speedLevel = speedLevel >> 4;
        Serial.println(speedLevel);
        secureBit = bitRead(receivedNumber, 0); //2nd parameter is nth position of bit from right
        nthValue = 1;
        Serial.println("Case 2 completed");
        break;
    }
  }

  //OR bit algebra operation: connect last 5 bits of first byte and first 3 bits of second byte to 1 byte
  double receivedAngle = firstHalfDegrees | secondHalfDegrees;
  receivedAngle = round(receivedAngle * CONV_DEGREES);
  speedModifier = round(speedLevel * STEP_MULTIPLIER);

  Serial.println(instructionPrimitiveCode);
  Serial.println(receivedAngle);
  Serial.println(instructionPrimitiveCode);

  //Now we have all parts we need to rock. Lets Move!
  
  //========================== ACTION WITH MOTORS ================================
  switch(instructionPrimitiveCode){
    
    //////////////////////////////////////////////////
      /////////////// Go Forward: 111 //////////////
    //////////////////////////////////////////////////
    
    case 7: //or '111' code for 'Go' instruction
    {
      Serial.println("Started 'GO' instruction");
      
      //Convert degrees to radians to use math functions correctly
      receivedAngle = receivedAngle * CONV_DEG_TO_RAD;
      double vectorX = cos(receivedAngle);
      double vectorY = sin(receivedAngle);
 
      if (speedLevel == 0) speedLevel = 1;
      
      //Calculate SPS for each motor <0,450>
      double motor1Speed = (-1 * vectorX) * speedModifier;
      double motor2Speed = (0.5 * vectorX + sqrt(3)/2 * vectorY) * speedModifier;
      double motor3Speed = (0.5 * vectorX - sqrt(3)/2 * vectorY) * speedModifier;

      //Set speed by RPS for each motor
      motor1.setSpeed(motor1Speed); motor2.setSpeed(motor2Speed); motor3.setSpeed(motor3Speed);
            
      // Run all motors for 3 seconds
      enableMotorsPower();
      int i=0;
      while(i<3000)
      {
        motor1.runSpeed(); motor2.runSpeed(); motor3.runSpeed();
        i++;
        delay(1);
      }
      disableMotorsPower();
      
      Serial.println("Ended 'GO' instruction");
    }
    break; //<- case 7: "Go" primitive instruction END.

    //////////////////////////////////////////////////
      /////////////// Rotating: 101 ///////////////
    //////////////////////////////////////////////////
    
    case 5: //or '101' code for 'Rotate' instruction
    {
      Serial.println("Started 'ROTATE' instruction");
       
      //speedLevel is here 0 or 1. 0 for rotation left(+ motors values), 1 rotation right(- motors values)
      //rotating left
      if (speedLevel == 0) { 
        int finishAngle =  round(receivedAngle * MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION); 
        motor1.setSpeed(speedModifier); motor2.setSpeed(speedModifier); motor3.setSpeed(speedModifier);
      }
       //rotating right
      else if (speedLevel == 1){
        int finishAngle = -1*round(receivedAngle * MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION); 
        motor1.setSpeed(-1*speedModifier); motor2.setSpeed(-1*speedModifier); motor3.setSpeed(-1*speedModifier);
      }
      
      motor1.setCurrentPosition(0); motor2.setCurrentPosition(0); motor3.setCurrentPosition(0);
      enableMotorsPower();
      // Running motors. Controlling just motor1 position, because they all run the same
      while(motor1.currentPosition() != finishAngle){
        motor1.runSpeed(); motor2.runSpeed(); motor3.runSpeed();
        delay(1);
      }
      disableMotorsPower();
      
      Serial.println("Ended 'ROTATE' instruction");
    }  
    break; //<- case 5: "Rotate" primitive instruction END.

    //////////////////////////////////////////////////
      /////////////// Braking: 100 ///////////////
    //////////////////////////////////////////////////
    // ######## USE CAREFULLY. LONG BRAKE IS GONNA BURN OUT THE MOTORS ##########
    case 4: //or '100' code for 'Brake' instruction
    {
      Serial.println("Started 'BRAKE' instruction");
      //Just enabling power to stepper motors and not moving them. 
      //Because they are powered up and holding their position TIGHT
      enableMotorsPower();
      Serial.println("Ended 'BRAKE' instruction");
    }  
    break; //<- case 4: "Brake" primitive instruction END.

    //////////////////////////////////////////////////
      /////////////// setSpeed: 001 ///////////////
    //////////////////////////////////////////////////
    
    case 1: //or '001' code for 'setSpeed' instruction
    {
      Serial.println("Started 'SETSPEED' instruction");
      motor1.setSpeed(speedModifier); motor2.setSpeed(speedModifier); motor3.setSpeed(speedModifier);
      Serial.println("Ended 'SETSPEED' instruction");
    }  
    break; //<- case 0: "setSpeed" primitive instruction END.

    //////////////////////////////////////////////////
      /////////////// Stopping: 000 ///////////////
    //////////////////////////////////////////////////
    
    case 0: //or '000' code for 'Stop' instruction
    {
      Serial.println("Started 'STOP' instruction");
      disableMotorsPower();
      Serial.println("Ended 'STOP' instruction");
    }  
    break; //<- case 0: "Stop" primitive instruction END.
    
  } //<- switch(instructionPrimitiveCode) END.
  
  movementCompleted = 1;
} //<- void receiveData() END.

//Actually the AccelStepper library has like inverted functions for enable/disable motors usage
//AccelStepper::motor1.enableOutputs() actually disables, disableOutputs() enables... funny huh?
//Function turns OFF the power to the motor coils to not burn them, saving power too
void disableMotorsPower(){
  motor1.enableOutputs(); 
  motor2.enableOutputs();
  motor3.enableOutputs();
}

//Function turns ON the power to the motor coils to not burn them, saving power too
void enableMotorsPower(){
  motor1.disableOutputs(); 
  motor2.disableOutputs();
  motor3.disableOutputs();
}

// Maybe for future usage - function to convert decimal to binary 
String decToBinary(byte n) 
{ 
  if(n==0){
    return "0";  
  }
  else{
    String binar = "";
    while(n>0){
      binar = n%2 + binar;
      n = n/2;  
    }
    return binar;
  }
} 

    //######################################################
      //####### SENDING REQUESTED DATA TO RPI ##########
    //######################################################
    
void sendData() {
  Wire.write(movementCompleted);
}
