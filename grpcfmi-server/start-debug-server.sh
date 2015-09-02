#!/bin/bash
java -jar ../grpcfmi-server/target/fmi-server-7-jar-with-dependencies.jar &
pid=$!
echo $pid
echo $pid > server.pid
sleep 1
