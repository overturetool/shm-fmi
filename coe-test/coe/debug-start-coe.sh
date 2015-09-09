#!/bin/bash
#export LD_LIBRARY_PATH=../../libgrpcfmi
java -Xdebug -Xrunjdwp:transport=dt_socket,address=8999,server=y -jar coe-0.0.1-SNAPSHOT-jar-with-dependencies.jar
