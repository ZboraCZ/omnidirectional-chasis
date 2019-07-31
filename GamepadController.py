import RPi.GPIO as GPIO
import pygame
import math
import time
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

    # Correct X movement value by recalculating its wrong value caused by little incompatibility
    def calculateLeftXaxisValueByPythagorasTheorem(self, leftXaxis, leftYaxis):
        return math.sqrt(1-math.pow(leftYaxis, 2)) * leftXaxis

    # Correct Y movement value by recalculating its wrong value caused by little incompatibility
    def calculateLeftYaxisValueByPythagorasTheorem(self, leftXaxis, leftYaxis):
        return math.sqrt(1-math.pow(leftXaxis, 2)) * leftYaxis

    # Calculate speeds (velocities of all wheels)
    # params leftXaxis: GO LEFT ; RIGHT (-1.0 : 1.0)
    # leftYaxis: GO FORWARD ; BACKWARD (-1.0 : 1.0)
    # rightXaxis: ROTATE LEFT ; RIGHT -1.0 ; 1.0
    def calculateVelocitiesOfWheels(self, leftXaxis, leftYaxis, rightXaxis):
        if not (abs(leftXaxis) == 0 and abs(leftYaxis) == 0 and abs(rightXaxis) == 0):

            # Jak rychle chceme jet => Jak moc je vyklonena packa
            magnitude = math.sqrt( math.pow(leftXaxis, 2) + math.pow(leftYaxis, 2) )
            if magnitude > 1: magnitude = 1

            # Predejiti uplnym nulam u smeru vektoru jizdy-Tyto vysledne nuly by mely nezadouci ucinky, popripade Vyjimky
            if leftXaxis == 0: leftXaxis = 0.0000001
            if leftYaxis == 0: leftYaxis = 0.0000001

            # Smer vektoru jizdy => uhel packy... neboli uhel od vodorovne osy x => THETA
            direction = math.atan( leftYaxis/leftXaxis )

            # Kdyz se jede doleva, uhel vychyleni od osy X se odecita od PI(180°)
            if leftXaxis < 0: direction = math.pi - direction
            # Kdyz se jede doprava, uhel vychyleni od osy X se odecita od nuly(0°)
            # POZNAMKA: Kdyz leftXaxis bylo na zacatku 0, nastavilo se vyse na 0.0000001, tedy spadne do teto vetve
            elif leftXaxis > 0: direction = 0 - direction
            
            #Vypocet vektoru X a vektoru Y
            vectorX = math.cos(direction) * magnitude
            vectorY = math.sin(direction) * magnitude
            
            print('magnitude: ' + str(magnitude))
            print('direction: ' + str(direction))
            print('vectorX: ' + str(vectorX) + '; vectorY: ' + str(vectorY))

            # nastaveni finalniho otaceni na motorky
            # Arduino zadava motorkum rozpeti od 0 do 255, proto nasobime desetinne cislo 255kou
            # zaokrouhluje se na cela cisla, kvuli I2C prenosu
            motor1Speed = -1 * (vectorX)
            motor2Speed = 0.5*vectorX + math.sqrt(3/2)*vectorY
            motor3Speed = 0.5*vectorX - math.sqrt(3/2)*vectorY

            # Ujisteni ze zadna hodnota nepresahuje plnou rychlost (1 nebo -1) a pote vynasobeni 255kou
            # Je predpokladano, ze v else nad 2ku se zadnej speed nedostane, proto int() -> odebere desetinne casti
            if abs(motor1Speed) <= 1:
                motor1Speed = int(round(motor1Speed * 255, 0))
            else:
                motor1Speed = int(motor1Speed) * 255

            if abs(motor2Speed) <= 1:
                motor2Speed = int(round(motor2Speed * 255, 0))
            else:
                motor2Speed = int(motor2Speed) * 255

            if abs(motor3Speed) <= 1:
                motor3Speed = int(round(motor3Speed * 255, 0))
            else:
                motor3Speed = int(motor3Speed) * 255


            print(str(motor1Speed) + ' ' + str(motor2Speed) + ' ' + str(motor3Speed) + ' | ' + str(rightXaxis))

            # self.sendInstructions(motor1Speed, motor2Speed, motor3Speed, rightXaxis)

    def sendInstructions(self, motor1speed, motor2speed, motor3speed, rightXaxis):
        # TODO - send axis for movement to Arduino
#        bus = smbus.SMBus(1)
        address = 0x04
#        bus.write_byte(address, motor1speed)
#        bus.write_byte_data(address, 0, motor1speed)
#        
#        bus.write_byte(address, motor2speed)
#        bus.write_byte_data(address, 0, motor2speed)
#        
#        bus.write_byte(address, motor3speed)
#        bus.write_byte_data(address, 0, motor3speed)
#        
#        bus.write_byte(address, rightXaxis)
#        bus.write_byte_data(address, 0, rightXaxis)
#        time.sleep(1)
        
        data = rightXaxis
        try:
            with SMBusWrapper(1) as bus:
                #Write a block of some bytes to address ...
                bus.write_byte(address, data) 
                bus.write_i2c_block_data(address, 0, data)