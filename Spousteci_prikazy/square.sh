#!/usr/bin/python
#TO-TEST: Maybe it has to be #!/bin/bash or #!/usr/bin/bash or has to be even elsewhere to run CORRECTLY.
#Script for bachelors work moving in squares and rotations.
print("Started SQUARE combined instruction")

#Going square anticlockwise
python go.sh 90 16
python go.sh 180 16
python go.sh 270 16
python go.sh 0 16

#Rotating 360 left
python rotate.sh 360 0

#Rotating 360 right
python rotate.sh 360 1

#Going square clockwise
python go.sh 90 16
python go.sh 0 16
python go.sh 270 16
python go.sh 180 16

print("Ended SQUARE combined instruction")