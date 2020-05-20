#!/bin/bash

executer=linux-x64-dockcross

if [ ! -f "$executer" ]; then

docker run --rm dockcross/linux-x64:latest >$executer
chmod +x $executer

fi



cd $(git rev-parse --show-toplevel)

./scripts/compile-linux.sh $executer x64



