#!/bin/bash

executer=windows-static-x64

if [ ! -f "$executer" ]; then

docker run --rm dockcross/windows-static-x64-posix >$executer
chmod +x $executer

fi



cd $(git rev-parse --show-toplevel)

./scripts/compile-win.sh $executer x64



