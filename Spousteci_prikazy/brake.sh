#!/usr/bin/python

import sys
import math
import time
import smbus

ARDUINO_ADDRESS = 0x04

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

################# START OF THE PROGRAM ######################

try:
   finalAngle = 0 #max. 255
   speedLevel = 0

   finalAngle = decToBinary(finalAngle, 8)
   speedLevel = decToBinary(speedLevel, 4)

   firstBit = '100' + finalAngle[:5]
   secondBit = finalAngle[5:] + speedLevel + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))

   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]

   bus.write_block_data(ARDUINO_ADDRESS, 0, data)
   #waitForArduinoCompletion()


