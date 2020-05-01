#!/usr/bin/python

import smbus
import time
# for RPI version 1, use bus = smbus.SMBus(0)
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

def writeNumber(value):
	# bus.write_byte(address, value)
	firstBit = 255
   	secondBit = 15
   	value = [firstBit, secondBit]
	bus.write_block_data(address, 0, value)
	return -1

def readNumber():
	number = bus.read_byte(address)
	# number = bus.read_byte_data(address, 1)
	return number

while True:
	var = input("Enter 1 to 9 ")
	if not var:
		continue
	
	firstBit = "11111111"
   	secondBit = "11111111"
	
	print firstBit
   	print secondBit

	value = [int(firstBit,2), int(secondBit,2)]
	bus.write_block_data(address, 0, value)

	print "RPI: Hi Arduino, I sent you "
	# sleep one second
	time.sleep(3)

	#number = readNumber()
	print "Arduino: Hey RPI, I received a digit ", number
	print
print "End of program"
