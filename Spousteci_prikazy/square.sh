#!/bin/sh
#maybe !/bin/bash
#maybe !/usr/bin/python
#TO-TEST: Maybe it has to be #!/bin/bash or #!/usr/bin/bash or has to be even elsewhere to run CORRECTLY.
#Script for bachelors work moving in squares and rotations.
echo -e "Started SQUARE combined instruction"

#Going square anticlockwise
python go.sh 90 15
python go.sh 180 15
python go.sh 270 15
python go.sh 0 15

#Rotating 360 left
python rotate.sh 360 0

#Rotating 360 right
python rotate.sh 360 1

#Going star clockwise
python go.sh 135 15
python go.sh 45 15
python go.sh 315 15
python go.sh 225 15

echo -e "Ended SQUARE combined instruction"