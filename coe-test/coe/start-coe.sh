#!/bin/bash

if [ -f "coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar" ]
then
echo "COE is present"
else
echo "Downloading COE, old fixed version for now"
wget http://overture.au.dk/into-cps/development/latest/coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar
fi

java -jar coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar
