#!/bin/sh
#maybe !/bin/bash
#maybe !/usr/bin/python
#TO-TEST: Maybe it has to be #!/bin/bash or #!/usr/bin/bash or has to be even elsewhere to run CORRECTLY.
#Script for bachelors work moving in squares and rotations.
printf "Started SQUARE combined instruction\n"

#Going square anticlockwise
python go.sh 90 15
python go.sh 180 15
python go.sh 270 15
python go.sh 0 15

#Rotating 360 left
python rotate.sh 360 0

#Rotating 360 right
python rotate.sh 360 1

#Going square clockwise
python go.sh 90 15
python go.sh 0 15
python go.sh 270 15
python go.sh 180 15

#Try braking and then stop
python brake.sh
python stop.sh

printf "Ended SQUARE combined instruction\n"