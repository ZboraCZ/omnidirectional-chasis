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
   print("final angle: " +str(finalAngle))

   data = [finalAngle, distance]
   
   #bus.write_block_data(ARDUINO_ADDRESS, 1, data) 
   bus.write_block_data(ARDUINO_ADDRESS, 1, [0b11111111,0b00001010]) 

except IndexError:
   print("Angle, distance parameters required")




