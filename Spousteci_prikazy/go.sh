#!/usr/bin/python

import sys
import math
import time
import smbus

ARDUINO_ADDRESS = 0x04

#const for converting 360 degrees to 1 byte(255)
CONV_DEG = 1.4117647058823529411764

bus = smbus.SMBus(1)

def decToBinary(decNumber, numBinPlaces):
	if decNumber == 0: 
		binaryNum = "0"
	else:
		binaryNum = ""
 		while decNumber>0:
    			binaryNum = str(decNumber%2) + binaryNum
    			decNumber = decNumber/2
  
	while len(binaryNum) < numBinPlaces:
		binaryNum = '0' + binaryNum
  	return binaryNum

# TODO - NAVRH CEKANI NA DOKONCENI AKCE NA ARDUINU #####
def waitForArduinoCompletion():
	time.sleep(3.4)
	#while not isArduinoDone():
	#	print("Arduino still not done")
	#	time.sleep(6.5)
	#print('Script-Info: Arduino completed GO instruction ' + str(firstBit) + ' ' + str(secondBit))

#def isArduinoDone():
#	number = bus.read_byte(ARDUINO_ADDRESS)
#	# number = bus.read_byte_data(address, 1)
#	return True if number==1 else False

def checkSpeedLevel(speedLevel):
	if speedLevel < 0:
		return 0
	elif speedLevel > 15:
		return 15
	else:
		return speedLevel

def checkAngle(angle):
	while angle > 360:
      		angle -= 360
	while angle < 0:
      		angle += 360

################# START OF THE PROGRAM ######################

try:
   print("=====================")
   print("Running GO command")

   angle = int(sys.argv[1])      #uhel jizdy ve stupnich [0-360]
   speedLevel = int(sys.argv[2])   #Uroven rychlosti [0-15]

   #Making sure we are in 0-360 angle radius
   checkAngle(angle)
   #Making sure we are in 0-15 radius
   speedLevel = checkSpeedLevel(speedLevel)

   finalAngle = int(round(angle/CONV_DEG)) #max. 255

   print("final angle, speedLevel: " +str(finalAngle) + ' ' + str(speedLevel) + ' | ' + decToBinary(finalAngle, 8) + ' ' + decToBinary(speedLevel, 4))
   
   finalAngle = decToBinary(finalAngle, 8)
   speedLevel = decToBinary(speedLevel, 4)

   firstBit = '111' + finalAngle[:5]
   secondBit = finalAngle[5:] + speedLevel + '1'

   print('Binary first bit, second bit: ' + firstBit + ' ' + secondBit)

   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   
   bus.write_block_data(ARDUINO_ADDRESS, 0, data)
   waitForArduinoCompletion()

except IndexError:
   print("Parameters required: Angle 0-360; speedLevel 0-15")

