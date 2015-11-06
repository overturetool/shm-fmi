#!/bin/bash
set -e

out_bin_root=builds/output/fmu/binaries
#out_server_lib=builds/output/sharedmemory-server
out_server_lib=shmfmi-server/target/classes/lib/

mkdir -p $out_bin_root
mkdir -p $out_server_lib

if [ "$1" = "clean" ]; then
   echo Cleaning
rm -r $out_bin_root
rm -r $out_server_lib


echo clean linux 64
make -C builds/linux64/shm clean

echo clean win32 
make -C builds/win32/shm clean

echo clean win64
make -C builds/win64/shm clean

else


echo building linux 64
make -C builds/linux64/shm
make -C builds/linux64/shm test
mkdir -p $out_bin_root/linux64/
cp builds/linux64/shm/libshmfmu/liblibshmfmu.so $out_bin_root/linux64/libshmfmu.so
mkdir -p $out_server_lib/Linux-amd64/
cp builds/linux64/shm/shmfmi-server/libsharedmemory.so $out_server_lib/Linux-amd64/libsharedmemory.so

echo building win32 
make -C builds/win32/shm/libshmfmu
make -C builds/win32/shm/shmfmi-server

mkdir -p $out_bin_root/win32/
cp builds/win32/shm/libshmfmu/libshmfmu.dll $out_bin_root/win32/

mkdir -p $out_server_lib/Windows-x86/
cp builds/win32/shm/shmfmi-server/sharedmemory.dll $out_server_lib/Windows-x86/



echo building win64
make -C builds/win64/shm/libshmfmu
make -C builds/win64/shm/shmfmi-server

mkdir -p $out_bin_root/win64/
cp builds/win64/shm/libshmfmu/libshmfmu.dll $out_bin_root/win64/
mkdir -p $out_server_lib/Windows-amd64/
cp builds/win64/shm/shmfmi-server/sharedmemory.dll $out_server_lib/Windows-amd64/


fi
