#!/bin/bash
java -jar ../grpcfmi-server/target/fmi-server-7-jar-with-dependencies.jar >server.log 2>&1 &
pid=$!
echo $pid
echo $pid > server.pid
sleep 1
