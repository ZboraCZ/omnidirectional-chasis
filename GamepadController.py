import pygame


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

    def sendInstructions(self, leftXaxis, leftYaxis, rightXaxis):
        # TODO - send axis for movement to Arduino
        print('sending to arduino')

