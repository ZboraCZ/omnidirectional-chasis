#include <Wire.h>
#include <AccelStepper.h>

#define SLAVE_ADDRESS 0x04

//Const for converting degrees to radians for math calculations of motor movements
const double CONV_DEG_TO_RAD = 0.01745329251;
//Steps multiplier for max. speed level 15 from RPi into max of 240 SPS
const int STEP_MULTIPLIER = 16;

//MOTORS AND WHEELS DECLARATIONS 
// Diameter of wheels in cm
const double WHEELS_DIAMETER = 5.5;
//Wheels circumference in cm (Obvod kola)
const double WHEELS_CIRCUMFERENCE = WHEELS_DIAMETER * PI; //=17.27875
const double CHASIS_CIRCUMFERENCE = round(2 * PI * 18); // =113.097335529  18 is chasis radius ending at the center of wheel
const double CM_ON_ONE_DEGREE_CHASIS_ROTATION = CHASIS_CIRCUMFERENCE/360;  // = 0.35 [cm]
const double MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION = CM_ON_ONE_DEGREE_CHASIS_ROTATION/(WHEELS_CIRCUMFERENCE/200); //=7,292194287...   WHEELS_CIRCUMFERENCE/360 is about 0.047996527...
//Max steps per second for AccelSteper library
const int MAX_SPS = 240;

boolean instructionToDo = false;
byte instructionPrimitiveCode = 255;
double receivedAngle = 0;
//Unmodified received speedLevel. Is used for some identifications
byte speedLevel = 0;
//Modifier for all Motors speeds multiplications. 
//Goes to max 240, this is default, if "Go" doesnt go first to set it.
int speedModifier = 200;
byte secureBit = 1;

//##############################################################################
//####################### MOTOR DECLARATION SECTION ############################
//##############################################################################
// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define motorInterfaceType 1
const int M1_DIR_PIN = 2; const int M1_STEP_PIN = 3;
const int M2_DIR_PIN = 4; const int M2_STEP_PIN = 5;
const int M3_DIR_PIN = 7; const int M3_STEP_PIN = 6;

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

}

    //######################################################
      //########## APPLICATION MAIN LOOP #############
    //######################################################

void loop() {
if(instructionToDo){
  instructionToDo = false;
if(secureBit == 1){
    if(instructionPrimitiveCode == 7){
        speedModifier = round(speedLevel * STEP_MULTIPLIER);
        receivedAngle = receivedAngle * CONV_DEG_TO_RAD;
        double vectorX = cos(receivedAngle);
        double vectorY = sin(receivedAngle);
   
        if (speedLevel == 0) speedLevel = 1;
        
        enableMotorsPower();
        
        //Calculate SPS for each motor <0,240>
        double motor1Speed = (-1 * vectorX) * speedModifier;
        double motor2Speed = (0.5 * vectorX + sqrt(3)/2 * vectorY) * speedModifier;
        double motor3Speed = (0.5 * vectorX - sqrt(3)/2 * vectorY) * speedModifier;

        //Set speed by RPS for each motor
        motor1.setSpeed(motor1Speed); motor2.setSpeed(motor2Speed); motor3.setSpeed(motor3Speed);
        
        // Run all motors for 3 seconds      
        int i=0;
        unsigned long startMillis = millis();
        while((millis() - startMillis) < 3000)
        {
        
          motor1.runSpeed();
          motor2.runSpeed(); 
          motor3.runSpeed();  
          delay(1);
        }
        disableMotorsPower(); 
        
    } //GO INSTRUCTION END

    else if(instructionPrimitiveCode == 5){

        enableMotorsPower();

        //SETTING CURRENT POSITION RESETS SPEED TO 0. HAS TO BE BEFORE SETTING SPEED ON MOTORS!!!        
        motor1.setCurrentPosition(0); motor2.setCurrentPosition(0); motor3.setCurrentPosition(0);

        double finishPosition;
        //speedLevel is here 0 or 1. 0 for rotation left(+ motors values), 1 rotation right(- motors values)
        //rotating left
        if (speedLevel == 0) { 
          finishPosition =  round(receivedAngle * MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION); 
          motor1.setSpeed(speedModifier); motor2.setSpeed(speedModifier); motor3.setSpeed(speedModifier);
        }
         //rotating right
        else if (speedLevel == 1){
          finishPosition = -1*round(receivedAngle * MOTOR_STEPS_PER_ONE_CHASIS_DEGREE_ROTATION); 
          motor1.setSpeed(-1*speedModifier); motor2.setSpeed(-1*speedModifier); motor3.setSpeed(-1*speedModifier);
        }
        // Running motors. Controlling just motor1 position, because they all run the same
        while(motor1.currentPosition() != finishPosition){
          motor1.runSpeed(); motor2.runSpeed(); motor3.runSpeed();
          delay(1);
        }
        
        disableMotorsPower(); 
        
    } //ROTATE INSTRUCTION END

    else if(instructionPrimitiveCode == 4){
        //Just enabling power to stepper motors and not moving them. 
        //Because they are powered up and holding their position TIGHT
        enableMotorsPower();
    }

    else if(instructionPrimitiveCode == 1){
      speedModifier = round(speedLevel * STEP_MULTIPLIER);
      motor1.setSpeed(speedModifier); motor2.setSpeed(speedModifier); motor3.setSpeed(speedModifier);  
    }

    else if(instructionPrimitiveCode == 0){
        disableMotorsPower();
    }

} //if(secureBit == 1) END
} //if instructionToDo END

//Serial.println("Loop end.");
} //void loop() END


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



    //######################################################
      //########## RECEIVING DATA FROM RPI #############
    //######################################################
    
// Process received bytes from RPi to move
void receiveData(int byteCount) {
  //Const for converting 1 byte(max 255) back to 360 degrees sent from RPi
  const double CONV_DEGREES = 1.4117647058823529411764; //MUST MATCH CONST ON RPi for valid nums.
  // this is the acutal being read number from the I2C bus
  byte receivedNumber = 0;
  // We are getting 2 values.
  //1st byte is 3 bits of instruction primitive (go, turn, rotate, stop,...) and 5 bits of angle 
  //2nd byte is another 3 bits of angle, 4 bits of number in centimeters, last is Security bit
  int nthValue = 1;
  
  int offsetIndexesOfBytes = byteCount/2; // each offset is for 1 byte... using this int in FOR loop to jump over them as they are starting nums.
  
  byte m_instructionPrimitiveCode;
  byte m_firstHalfDegrees;
  byte m_secondHalfDegrees;
  byte m_receivedAngle;
  byte m_speedLevel;
  byte m_secureBit;
  /* Debugging
  for(int i =0; i<=byteCount; i++){ //hopping over the starting offset indexes
      receivedNumber = Wire.read();
      /*
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
        m_instructionPrimitiveCode = receivedNumber >> 5;
        m_firstHalfDegrees = receivedNumber << 3;
        nthValue++;
        break; 

      //Second byte arrived (3 bits: second half of middle number; 4 bits: speedLevel measurement [cm], 1 bit: secure bit)
      case 2: 
        m_secondHalfDegrees = receivedNumber >> 5;
        m_speedLevel = receivedNumber << 3; m_speedLevel = m_speedLevel >> 4;
        m_secureBit = bitRead(receivedNumber, 0); //2nd parameter is nth position of bit from right
        nthValue = 1;
        break;
    }
  }

  //OR bit algebra operation: connect last 5 bits of first byte and first 3 bits of second byte to 1 byte
  m_receivedAngle = m_firstHalfDegrees | m_secondHalfDegrees;

  //FINAL VARIABLES ASSIGNMENTS
  instructionPrimitiveCode = m_instructionPrimitiveCode;
  receivedAngle = round(m_receivedAngle * CONV_DEGREES);
  speedLevel = m_speedLevel;
  secureBit = m_secureBit;
  
  instructionToDo = true;
  
} //End of setting received data
