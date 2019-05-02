#!/bin/bash 
#COMMAND TO RUN
#bash README.txt
COUNTER=0
while [  $COUNTER -lt 10 ]; do
find . -name "*"$COUNTER".txt" -print0 | xargs -0 rm
find . -name "*"$COUNTER".jpg" -print0 | xargs -0 rm
let COUNTER=COUNTER+2
done
