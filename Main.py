# README: First you need to connect gamepad ->  $ sudo ds4drv
# poznamka: Shortcut pro komentovani/odkomentovani bloku kodu -> Ctrl + #
import pygame

from DisplayController import DisplayController
from GamepadController import GamepadController


# Initialize pyGame library for PS4 controller to use it
pygame.init()

# Loop until the user clicks the close button.
applicationFinished = False

# Manage how fast the screen updates
clock = pygame.time.Clock()

# Instantiate class for printing to app window and control of the Gamepad
displayController = DisplayController()
gamepadController = GamepadController()


# -------- Main Application Loop -----------
while applicationFinished is False:
    # EVENT PROCESSING STEP
    for event in pygame.event.get():  # User did something
        if event.type == pygame.QUIT:  # If user clicked close
            applicationFinished = True  # Flag that we are done so we exit this loop

        # Possible joystick actions: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN JOYBUTTONUP JOYHATMOTION
        if event.type == pygame.JOYBUTTONDOWN:
            print("Joystick button pressed.")
        if event.type == pygame.JOYBUTTONUP:
            print("Joystick button released.")

    # First, clear the screen to white. Other drawing commands above this will be erased with this command.
    displayController.fillScreen('white')

    # set default position and line height of window
    displayController.reset()

    displayController.print("Number of joysticks: {}".format(gamepadController.joystick_count))
    displayController.indent()

    # For each joystick:
    for i in range(gamepadController.joystick_count):
        joystick = gamepadController.getJoystick(i)
        joystick.init()

        displayController.print("Joystick {}".format(i))
        displayController.indent()

        # Get the name from the OS for the controller/joystick
        joystickFactoryName = joystick.get_name()
        displayController.print("Joystick name: {}".format(joystickFactoryName))


        ##### CUSTOM EDIT TO GET AXES AND ITS AXIS #####

        # Blank line for block separation
        displayController.print("")

        ###LEFT JOYSTICK###

        # GO LEFT ; RIGHT -1.0 ; 1.0 #
        leftXaxis = round(joystick.get_axis(0), 3)
        displayController.print("GO LEFT ; RIGHT (-1.0 : 1.0): {}".format(leftXaxis))
        displayController.print("")

        # GO FORWARD ; BACKWARD -1.0 ; 1.0 #
        leftYaxis = round(joystick.get_axis(1), 3)
        displayController.print("GO FORWARD ; BACKWARD (-1.0 : 1.0): {}".format(leftYaxis))
        displayController.print("")

        ###RIGHT JOYSTICK###
        # ROTATE LEFT ; RIGHT -1.0 ; 1.0 #
        rightXaxis = round(joystick.get_axis(2), 3)
        displayController.print("ROTATE LEFT ; RIGHT (-1.0 : 1.0): {}".format(rightXaxis))
        displayController.print("")

        # TODO - calculate values and send them to arduino
        gamepadController.sendInstructions(leftXaxis, leftYaxis, rightXaxis)

        ##### END OF MY CUSTOM EDIT FOR AXES #####

    # ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT

    # Go ahead and update the whole screen with what we've drawn.
    pygame.display.flip()

    # Limit to 60 frames per second
    clock.tick(60)

# Close the window and quit.
# If this line forgotten, the program will 'hang' on exit if running from IDLE.
pygame.quit()

