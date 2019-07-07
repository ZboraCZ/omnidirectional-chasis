# README: First you need to connect gamepad ->  $ sudo ds4drv
# poznamka: Shortcut pro komentovani/odkomentovani bloku kodu -> Ctrl + #
import pygame

from DisplayController import DisplayController

pygame.init()

#Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joystick
pygame.joystick.init()
    
# Instantiate class for printing
displayController = DisplayController()

# -------- Main Program Loop -----------
while done==False:
    # EVENT PROCESSING STEP
    for event in pygame.event.get(): # User did something
        if event.type == pygame.QUIT: # If user clicked close
            done=True # Flag that we are done so we exit this loop
        
        # Possible joystick actions: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN JOYBUTTONUP JOYHATMOTION
        if event.type == pygame.JOYBUTTONDOWN:
            print("Joystick button pressed.")
        if event.type == pygame.JOYBUTTONUP:
            print("Joystick button released.")
            
 
    # DRAWING STEP
    # First, clear the screen to white. Don't put other drawing commands
    # above this, or they will be erased with this command.
    displayController.fillScreen('white')

    # set default position and line height of window
    print('Reset window to default position:')
    displayController.reset()

    # Get count of joysticks
    joystick_count = pygame.joystick.get_count()

    displayController.print("Number of joysticks: {}".format(joystick_count) )
    displayController.indent()
    
    # For each joystick:
    for i in range(joystick_count):
        joystick = pygame.joystick.Joystick(i)
        joystick.init()
    
        displayController.print("Joystick {}".format(i) )
        displayController.indent()
    
        # Get the name from the OS for the controller/joystick
        name = joystick.get_name()
        displayController.print("Joystick name: {}".format(name) )
        
        
        ##### MY CUSTOM EDIT TO GET AXES AND ITS AXIS #####
        
        displayController.print("")
        ###LEFT JOYSTICK###
        
        # GO LEFT ; RIGHT -1.0 ; 1.0 # 
        axis = round( joystick.get_axis( 0 ), 3 )
        displayController.print("GO LEFT ; RIGHT: {}".format(axis) )
        displayController.print("")
        
        # GO FORWARD ; BACKWARD -1.0 ; 1.0 # 
        axis = round( joystick.get_axis( 1 ), 3 )
        displayController.print("GO FORWARD ; BACKWARD: {}".format(axis) )
        displayController.print("")
        
        ###RIGHT JOYSTICK###
        # ROTATE LEFT ; RIGHT -1.0 ; 1.0 # 
        axis = round( joystick.get_axis( 2 ), 3 )
        displayController.print("ROTATE LEFT ; RIGHT: {}".format(axis) )
        displayController.print("")

        ##### END OF MY CUSTOM EDIT FOR AXES #####

    # ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT
    
    # Go ahead and update the whole screen with what we've drawn.
    pygame.display.flip()

    # Limit to 60 frames per second
    clock.tick(60)
    
# Close the window and quit.
# If you forget this line, the program will 'hang'
# on exit if running from IDLE.
pygame.quit ()