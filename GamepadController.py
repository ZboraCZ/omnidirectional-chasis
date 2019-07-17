import RPi.GPIO as GPIO
import pygame
import math
from time import sleep
from smbus2 import SMBusWrapper


class GamepadController:
    # Constructor for whole class controlling all the gamepads
    def __init__(self):
        # Initialize the functionalities with joysticks
        pygame.joystick.init()

        # Attribute Count of joysticks
        self.joystick_count = pygame.joystick.get_count()

    # Get specific Joystick instance of the joystick list
    def getJoystick(self, idNumber):
        return pygame.joystick.Joystick(idNumber)
    
    # Calculate speeds (velocities of all wheels)
    #params leftXaxis: GO LEFT ; RIGHT (-1.0 : 1.0)
    #leftYaxis: GO FORWARD ; BACKWARD (1.0 : -1.0)
    #rightXaxis: ROTATE LEFT ; RIGHT -1.0 ; 1.0 
    def calculateVelocitiesOfWheels(self, leftXaxis, leftYaxis, rightXaxis):
        if not (abs(leftXaxis) == 0 and abs(leftYaxis) == 0 and abs(rightXaxis) == 0):
            # Jak rychle chceme jet => Jak moc je vyklonena packa
            magnitude = math.sqrt( math.pow(leftXaxis, 2) + math.pow(leftYaxis, 2) )
            if magnitude > 1: magnitude = 1
            
            try:
                # Smer vektoru jizdy => uhel packy... neboli uhel od vodorovne osy x => THETA
                direction = math.atan( leftYaxis/leftXaxis )
            except ZeroDivisionError as err:
                direction = 0
            
            #Vypocet vektoru X a vektoru Y
            vectorX = math.cos(direction) * magnitude
            vectorY = math.sin(direction) * magnitude
            
            print('magnitude: ' + str(magnitude))
            print('direction: ' + str(direction))
            print('vectorX: ' + str(vectorX))
            
            #nastaveni finalniho otaceni na motorky
            #Arduino zadava motorkum rozpeti od 0 do 255, proto nasobime desetinne cislo 255kou
            #zaokrouhluje se na cela cisla, kvuli I2C prenosu
            motor1Speed = int( round((vectorX) * 255 , 0) )
            motor2Speed = int( round((0.5*vectorX + math.sqrt(3/2)*vectorY) * 255 , 0) )
            motor3Speed = int( round((0.5*vectorX - math.sqrt(3/2)*vectorY) * 255 , 0) )
            #change direction of Motor1 when going left side
            if leftXaxis > 0: motor1Speed = motor1Speed * -1
            if leftXaxis == 0: motor1Speed = 0;
            
            print(str(motor1Speed) + ' ' + str(motor2Speed) + ' ' + str(motor3Speed))
            

    #def sendInstructions(self, leftXaxis, leftYaxis, rightXaxis):
        # TODO - send axis for movement to Arduino
        #address = 30
        #data = [leftXaxis, leftYaxis, rightXaxis]
        #try:
            #with SMBusWrapper(1) as bus:
                # Write a block of some bytes to address ...
                #bus.write_i2c_block_data(address, 0, data)