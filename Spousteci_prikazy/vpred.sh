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
   distance = int(sys.argv[2])   #vzdalenost jizdy v cm [1-16]
   
   while angle > 360:
      angle -= 360

   finalAngle = int(round(angle/CONV_DEG))
   print("final angle, distance: " +str(finalAngle) + ' ' + str(distance) + ' | ' + bin(finalAngle)[2:]) + ' ' + bin(distance)[2:]
   
   finalAngle = bin(finalAngle)[2:]
   while (len(finalAngle) < 8): finalAngle = '0' + finalAngle 

   firstBit = '111' + finalAngle[:5]
   secondBit = finalAngle[5:] + bin(distance)[2:] + '1'

   print('Binary first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   firstBit = int(firstBit, 2)
   secondBit = int(secondBit, 2)

   data = [firstBit, secondBit]
   print('Sent first bit, second bit: ' + str(firstBit) + ' ' + str(secondBit))
   
   bus.write_block_data(ARDUINO_ADDRESS, 0, data) 

except IndexError:
   print("Angle, distance parameters required")




