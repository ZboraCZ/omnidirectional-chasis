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
        if abs(leftXaxis) < 0.1: leftXaxis=0.0

        # GO FORWARD ; BACKWARD -1.0 ; 1.0 #
        leftYaxis = round(joystick.get_axis(1), 3)
        # Pocita se s malymi odchylkami, kdyz jsu packy v klidu, at nam nelita podvozek
        if abs(leftYaxis) < 0.1: leftYaxis=0.0

        ###RIGHT JOYSTICK###
        # ROTATE LEFT ; RIGHT -1.0 ; 1.0 #
        rightXaxis = round(joystick.get_axis(2), 3)
        # Pocita se s malymi odchylkami, kdyz jsu packy v klidu, at nam nelita podvozek
        if abs(rightXaxis) < 0.1: rightXaxis=0.0

        # Recalculate wrong values, on 1 or -1 and not 0 on the other direction
        # Wrong values are caused by some little hardware incompatibility
        if abs(leftXaxis) == 1 and leftYaxis != 0:
            leftXaxis = gamepadController.calculateLeftXaxisValueByPythagorasTheorem(leftXaxis, leftYaxis)

        elif abs(leftYaxis) == 1 and leftXaxis != 0:
            leftYaxis = gamepadController.calculateLeftYaxisValueByPythagorasTheorem(leftXaxis, leftYaxis)

        ###### Print joystick values to the screen ######
        # Print LEFT TO RIGHT DIRECTION VALUE
        #first round the values again - after pythagoras recalcs, numbers are wide
        leftXaxis = round(leftXaxis, 3)
        leftYaxis = round(leftYaxis, 3)
        displayController.print("GO LEFT ; RIGHT (-1.0 : 1.0): {}".format(leftXaxis))
        displayController.print("")

        # Print FORWARD TO BACKWARD DIRECTION VALUE
        displayController.print("GO FORWARD ; BACKWARD (-1.0 : 1.0): {}".format(leftYaxis))
        displayController.print("")

        # Print LEFT TO RIGHT ROTATION VALUE
        displayController.print("ROTATE LEFT ; RIGHT (-1.0 : 1.0): {}".format(rightXaxis))
        displayController.print("")

        # TODO - calculate values and send them to arduino
        gamepadController.calculateVelocitiesOfWheels(leftXaxis, leftYaxis, rightXaxis)

        ##### END OF MY CUSTOM EDIT FOR AXES #####

    # ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT

    # Go ahead and update the whole screen with what we've drawn.
    pygame.display.flip()

    # Limit to 2.6 frames per second | 2.6 IS THE HIGHEST STABLE number
    # 2.6-cca sleep 384ms mezi loopy, 2.5-sleep 400ms mezi loopy
    clock.tick(2.6)

# Close the window and quit.
# If this line forgotten, the program will 'hang' on exit if running from IDLE.
pygame.quit()

