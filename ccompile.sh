#!/bin/bash
#set -x #echo on

echo Checking X compiler for windows mingw

if [ $(dpkg-query -W -f='${Status}' mingw-w64 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo Missing package mingw-w64
  apt-get install mingw-w64;
fi

if [ $(dpkg-query -W -f='${Status}' g++-mingw-w64 2>/dev/null | grep -c "ok installed") -eq 0 ];
then
  echo Missing package g++-mingw-w64
  apt-get install g++-mingw-w64;
fi


#####################################################################################
#	Win 32
#####################################################################################

echo Prepare X-compile

function prepareMingw()
{

ARCH=$1
COMPILER=$2
TOOLCHAIN=$3

WIN32_BUILD_DIR=builds/$ARCH

PROTOBUF_BUILD_DIR=$WIN32_BUILD_DIR/protobuf/


mkdir -p $PROTOBUF_BUILD_DIR
mkdir -p $WIN32_BUILD_DIR/protobuf-${ARCH}

PROTOBUF_WIN32=`cd "$PROTOBUF_BUILD_DIR/../protobuf-$ARCH"; pwd`

#echo $PROTOBUF_$ARCH

cd $PROTOBUF_BUILD_DIR

#pwd



#protobuf win32
cd ../../../third_party/protobuf/

if [ ! -f configure ]; then
    echo "Configure not found! running autogen"
	./autogen.sh
fi

#./autogen.sh
cd ../../$PROTOBUF_BUILD_DIR

pwd

../../../third_party/protobuf/configure --host=$COMPILER --build=i686-pc-linux-gnu --with-protoc=protoc --disable-shared --prefix=$PROTOBUF_WIN32
#--disable-tests --disable-failing-tests
make
make install

cd ../
mkdir -p shm
cd shm

pwd

cmake -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DPROTOBUF_INCLUDE_DIR=$PROTOBUF_WIN32/include -DPROTOBUF_LIBRARY=$PROTOBUF_WIN32/lib/libprotobuf.a ../../../

cd ../../../
pwd
}

echo WIN32
prepareMingw win32 i686-w64-mingw32 ../../../third_party/cmake-toolchains/Toolchain-Ubuntu-mingw32.cmake

echo WIN64
prepareMingw win64 x86_64-w64-mingw32 ../../../third_party/cmake-toolchains/Toolchain-Ubuntu-mingw64.cmake
