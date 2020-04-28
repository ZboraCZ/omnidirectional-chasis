#!/usr/bin/python
#TO-TEST: Maybe it has to be #!/bin/bash or #!/usr/bin/bash or has to be even elsewhere to run CORRECTLY.
#Script for bachelors work moving in squares and rotations.
print("Started SQUARE combined instruction")

#Going square anticlockwise
./go.sh 90 16
./go.sh 180 16
./go.sh 270 16
./go.sh 0 16

#Rotating 360 left
./rotate.sh 360 0

#Rotating 360 right
./rotate.sh 360 1

#Going square clockwise
./go.sh 90 16
./go.sh 0 16
./go.sh 270 16
./go.sh 180 16

print("Ended SQUARE combined instruction")