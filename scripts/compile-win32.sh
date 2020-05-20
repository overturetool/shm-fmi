#!/bin/bash

executer=windows-static-x86

if [ ! -f "$executer" ]; then

docker run --rm dockcross/windows-static-x86 >$executer
chmod +x $executer

fi



cd $(git rev-parse --show-toplevel)

./scripts/compile-win.sh $executer x32



