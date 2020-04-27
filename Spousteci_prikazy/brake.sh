#!/usr/bin/python

import sys
import math
import time
import smbus

ARDUINO_ADDRESS = 0x04

bus = smbus.SMBus(1)

try:
   finalAngle = 0 #max. 255
   speedLevel = 0

   finalAngle = bin(finalAngle)
   speedLevel = bin(speedLevel)

   while (len(finalAngle) < 8): finalAngle = '0' + finalAngle #Add up zeros to 255 in binary
   while (len(speedLevel) < 4): speedLevel = '0' + speedLevel #Add up zeros to 16 in binary

   firstBit = '100' + finalAngle[:5]
   secondBit = finalAngle[5:] + bin(speedLevel) + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))

   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   print('Sent first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))

   bus.write_block_data(ARDUINO_ADDRESS, 0, data)
   waitForArduinoCompletion()

except IndexError:
   print("Angle[0-360]; speedLevel[1-16] parameters required")

##### TODO - NÁVRH ČEKÁNÍ NA DOKONČENÍ AKCE NA ARDUINU #####
def waitForArduinoCompletion():
	while !isArduinoDone():
		time.sleep(0.2)
	print('Script-Info: Arduino completed BRAKE instruction' + str(firstBit) + ' ' + str(secondBit))

def isArduinoDone():
	number = bus.read_byte(ARDUINO_ADDRESS)
	# number = bus.read_byte_data(address, 1)
	return True if number==1 else False
