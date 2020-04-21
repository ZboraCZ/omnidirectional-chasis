#!/usr/bin/python

import sys
import math
import time
import smbus

ARDUINO_ADDRESS = 0x04

#const for converting 360 degrees to 1 byte(255)
CONV_DEG = 1.4117647058823529411764

bus = smbus.SMBus(1)

try:
   angle = int(sys.argv[1])      #uhel jizdy ve stupnich [0-360]
   speedLevel = int(sys.argv[2])   #Uroven rychlosti [1-16]
   
   while angle > 360:
      angle -= 360

   finalAngle = int(round(angle/CONV_DEG)) #max. 255

   print("final angle, speedLevel: " +str(finalAngle) + ' ' + str(speedLevel) + ' | ' + bin(finalAngle) + ' ' + bin(speedLevel))
   
   finalAngle = bin(finalAngle)
   speedLevel = bin(speedLevel)

   while (len(finalAngle) < 8): finalAngle = '0' + finalAngle #Add up zeros to 255 in binary
   while (len(speedLevel) < 4): speedLevel = '0' + speedLevel #Add up zeros to 16 in binary

   firstBit = '111' + finalAngle[:5]
   secondBit = finalAngle[5:] + bin(speedLevel) + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   print('Sent first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   bus.write_block_data(ARDUINO_ADDRESS, 0, data) 

except IndexError:
   print("Angle[0-360]; speedLevel[1-16] parameters required")
