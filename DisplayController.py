# This is a simple class that will help us print to the screen
# It has nothing to do with the joysticks, just outputting the information.
import pygame


class DisplayController:
    def __init__(self):

        # Define some colors
        self.BLACK = (0, 0, 0)
        self.WHITE = (255, 255, 255)

        # The width and height of the display screen [width,height]
        self.screenSize = [500, 700]
        self.screen = pygame.display.set_mode(self.screenSize)
        pygame.display.set_caption("Controller Overlay")

        # Set default position of window and text line height
        self.x = 10
        self.y = 10
        self.line_height = 15

        self.reset()
        self.font = pygame.font.Font(None, 23)

        # Set window title
        pygame.display.set_caption("Controller Overlay")

    def print(self, textString):
        textBitmap = self.font.render(textString, True, self.BLACK)
        self.screen.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height

    def reset(self):
        print('Resetting window to default position...')
        self.x = 10
        self.y = 10
        self.line_height = 15

    def indent(self):
        self.x += 10

    def unindent(self):
        self.x -= 10

    def fillScreen(self, color):
        if color == 'white':
            print("Filling screen to white...")
            self.screen.fill(self.WHITE)

        elif color == 'black':
            print('Filling screen to black...')
            self.screen.fill(self.BLACK)

        else:
            print('Color to fill screen not specified: exiting')
            pygame.quit()


