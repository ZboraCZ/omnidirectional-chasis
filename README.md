#TO-TRY!!
Arduino cant handle timing functions in receiveData function
Create another function outside to return millis() time!

#List of things to take into Lab
* Chasis
* 9V battery
* Soldering array and the invoice for uni
* Voltage checker
* Wireless Keyboard
* Soldering 
* Screwdrivers

# TODO list in the lab
* Take photo of RPi and Arduino for Thesis.
* Build again just i2c comm Arduino-RPi
	* Make all new scripts bash executables: chmod a+x /path/to/file or chmod 777 /path/to/file
	* TOTEST: Response scripts from Arduino back to Raspberry Pi (new waiting method in RPi scripts)
	* Test all Instruction primitives
	* Try running the SQUARE combined instruction
* Try to build complete chasis with all motors on non-soldering array, try to USE ALL 3 100uF transistors from Arduino set
* Take photo of complete chasis build for Thesis.
* Take video of the chasis move.
	* Make a square move anticlockwise, rotate 360 left, then 360 right. Go the square back clockwise.
* Try build the chasis with all motors on soldering array, try to USE ALL 3 100uF transistors from Arduino set if worked
* LETS ROLL!

# Bachelor thesis: Design and control of omnidirectional chasis.

## Running Raspberry Pi Shell scripts manual
### go.sh [int degrees] [int speedLevel]
int degrees: ° from 0 to 360 defining the way chasis will go. 0° is right, 90° is front.
int speedLevel: Number from 1 to 16 defining how quickly will the motors rotate up to 200 steps per second.

### rotate.sh [int degrees] [int direction]
int degrees: ° from 0 to 360 defining how much will the chasis rotate.
int direction: Number 0 or 1. 0 is rotation left(anticlockwise), 1 is rotation right(clockwise)

### brake.sh
Doesn't take arguments. Holds the motors still.

### stop.sh
Doesn't take arguments. Disables powering of the motors.

### setSpeed.sh [int speedLevel]
int speedLevel: Number from 1 to 16 defining how quickly will the motors rotate up to 200 steps per second.
Just sets speed of motors. Doesn't move anything.

## Checking I2C connection addresses table on Raspberry Pi:
sudo i2cdetect -y 1



# Maybe for future controlling with PS4 Dualshock Controller device
Scripts in the folder "ZZZ_unused_Controlling_motors_PS4_controller"
Workflow to start control of the Raspberry Pi:
1. sudo ds4drv, then start controller //Dualshock 4 controller will connect
2. start Main.py to start controlling