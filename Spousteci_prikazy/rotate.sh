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
   rotationDirection = int(sys.argv[2])   #Smer rotace 0=vlevo, 1=vpravo
   #Angle always +, just direction changes
   while angle < 0:
      angle += 360

   finalAngle = int(round(angle/CONV_DEG)) #max. 255

   print("final angle, rotationDirection: " +str(finalAngle) + ' ' + str(rotationDirection) + ' | ' + bin(finalAngle) + ' ' + bin(rotationDirection))
   
   finalAngle = bin(finalAngle)
   rotationDirection = bin(rotationDirection)

   while (len(finalAngle) < 8): finalAngle = '0' + finalAngle #Add up zeros to 255 in binary
   while (len(rotationDirection) < 4): rotationDirection = '0' + rotationDirection #Add up zeros to 16 in binary

   firstBit = '101' + finalAngle[:5]
   secondBit = finalAngle[5:] + bin(rotationDirection) + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   print('Sent first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   bus.write_block_data(ARDUINO_ADDRESS, 0, data) 

except IndexError:
   print("Angle[0-360]; rotationDirection[0=left, 1=right] parameters required")
