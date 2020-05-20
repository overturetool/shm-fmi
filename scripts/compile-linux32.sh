#!/bin/bash

executer=linux-x86-dockcross

if [ ! -f "$executer" ]; then

docker run --rm dockcross/linux-x86:latest >$executer
chmod +x $executer

fi



cd $(git rev-parse --show-toplevel)

./scripts/compile-linux.sh $executer x32



