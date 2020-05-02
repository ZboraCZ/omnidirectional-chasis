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
	time.sleep(10)
	while not isArduinoDone():
		time.sleep(10)
	print('Script-Info: Arduino completed GO instruction' + str(firstBit) + ' ' + str(secondBit))

def isArduinoDone():
	number = bus.read_byte(ARDUINO_ADDRESS)
	# number = bus.read_byte_data(address, 1)
	return True if number==1 else False

def checkRotationDirection(rotationDirection):
	if rotationDirection < 0:
		return 0
	elif rotationDirection > 1:
		return 1
	else:
		return rotationDirection

def checkAngle(angle):
	while angle > 360:
      		angle -= 360
	while angle < 0:
      		angle += 360

################# START OF THE PROGRAM ######################

try:
   angle = int(sys.argv[1])      #uhel jizdy ve stupnich [0-360]
   rotationDirection = int(sys.argv[2])   #Smer rotace 0=vlevo, 1=vpravo

   #Making sure we are in 0-360 radius
   checkAngle(angle)
   #Making sure we are in 0-1 radius
   rotationDirection = checkRotationDirection(rotationDirection)

   finalAngle = int(round(angle/CONV_DEG)) #max. 255

   print("final angle, rotationDirection: " +str(finalAngle) + ' ' + str(rotationDirection) + ' | ' + bin(finalAngle) + ' ' + bin(rotationDirection))
   
   finalAngle = decToBinary(finalAngle, 8)
   rotationDirection = decToBinary(rotationDirection, 4)

   firstBit = '101' + finalAngle[:5]
   secondBit = finalAngle[5:] + rotationDirection + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   
   bus.write_block_data(ARDUINO_ADDRESS, 0, data)
   #waitForArduinoCompletion()

except IndexError:
   print("Angle[0-360]; rotationDirection[0=left, 1=right] parameters required")

