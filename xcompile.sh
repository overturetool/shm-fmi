#!/bin/bash

echo Storing git info in 'git-info.txt'

INFO=git-info.txt
rm -f $INFO

set +e
git remote -v > $INFO
git show-ref >> $INFO
git rev-parse HEAD >> $INFO
set -e


set -e

./scripts/compile-linux32.sh
./scripts/compile-linux64.sh

./scripts/compile-win32.sh
./scripts/compile-win64.sh

./scripts/compile-mac.sh


echo Copy native libraries in place

LIB=shmfmi-server/target/classes/lib

rm -rf $LIB

mkdir -p $LIB/Mac-x86_64
mkdir -p $LIB/Linux-amd64
mkdir -p $LIB/Linux-i386
mkdir -p $LIB/Windows-amd64
mkdir -p $LIB/Windows-x86

echo Copying server libs

cp git-info.txt $LIB/
cp builds/mac/shmfmi-server/libsharedmemory.dylib $LIB/Mac-x86_64/
cp builds/linux-x64/shmfmi-server/libsharedmemory.so $LIB/Linux-amd64/
cp builds/linux-x32/shmfmi-server/libsharedmemory.so $LIB/Linux-i386/
cp builds/winx64/shmfmi-server/sharedmemory.dll $LIB/Windows-amd64/
cp builds/winx32/shmfmi-server/sharedmemory.dll $LIB/Windows-x86/

# tag server
INFO=shmfmi-server/target/classes/git-info-server.txt
rm -f $INFO

set +e
git remote -v > $INFO
git show-ref HEAD >> $INFO
git rev-parse --verify HEAD >> $INFO
set -e

echo Copy FMU libs


FMU=vdm-tool-wrapper
FMUBIN=$FMU/binaries
mkdir -p $FMUBIN/darwin64
mkdir -p $FMUBIN/linux64
mkdir -p $FMUBIN/linux32
mkdir -p $FMUBIN/win64
mkdir -p $FMUBIN/win32

cp git-info.txt $FMUBIN/
cp builds/mac/libshmfmu/liblibshmfmu.dylib $FMUBIN/darwin64/$FMU.dylib
cp builds/linu-x64/libshmfmu/liblibshmfmu.so $FMUBIN/linux64/$FMU.so
cp builds/linu-x32/libshmfmu/liblibshmfmu.so $FMUBIN/linux32/$FMU.so
cp builds/winx64/libshmfmu/libshmfmu.dll $FMUBIN/win64/$FMU.dll
cp builds/winx32/libshmfmu/libshmfmu.dll $FMUBIN/win32/$FMU.dll

cp -r $FMU $LIB