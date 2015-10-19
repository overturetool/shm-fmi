#!/bin/bash

if [ -f "coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar" ]
then
echo "COE is present"
else
echo "Downloading COE, old fixed version for now"
wget http://overture.au.dk/into-cps/development/Build-77_2015-10-16_00-00/coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar
fi

java -jar coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar
