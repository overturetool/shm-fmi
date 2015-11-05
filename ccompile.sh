#!/bin/bash
#set -x #echo on

repo_root=`pwd`
#update with full path
repo_root=`cd "$repo_root"; pwd`

echo Init submodues
git submodule update --init


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

echo Checking if protobuf sources are ready
cd third_party/protobuf/
if [ ! -f configure ]; then
    echo "Configure not found! running autogen"
	./autogen.sh
fi

cd $repo_root


echo Checking if natime protobuf is installed
if [ ! -f /usr/local/lib/libprotobuf.a ] || [ ! -f /usr/local/bin/protoc ]; then
	echo "protobuf not installed"

mkdir -p $repo_root/builds/linux64/protobuf
cd $repo_root/builds/linux64/protobuf
CXXFLAGS=-fPIC $repo_root/third_party/protobuf/configure
make
pwd
echo -e "\033[32m echo Please install using `make install && ldconfig /usr/local/lib`"
#exit
sudo make install
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
J_HOME=`cd "$4"; pwd`
echo "Javahome is now: ${J_HOME}"
echo "4-th were $4"
WIN32_BUILD_DIR=$repo_root/builds/$ARCH

#PROTOBUF_BUILD_DIR=$WIN32_BUILD_DIR/protobuf/


#mkdir -p $PROTOBUF_BUILD_DIR
#mkdir -p $WIN32_BUILD_DIR/protobuf-${ARCH}
# Create protbuf build dir
#PROTOBUF_BUILD_DIR=$WIN32_BUILD_DIR/protobuf-${ARCH}
PROTOBUF_BUILD_DIR=~/.protobuf/protobuf-${ARCH}
mkdir -p $PROTOBUF_BUILD_DIR
# convert to full path
PROTOBUF_WIN32=`cd "$PROTOBUF_BUILD_DIR"; pwd`



cd $PROTOBUF_BUILD_DIR

if [ ! -f $PROTOBUF_BUILD_DIR/lib/libprotobuf.a ]; then
	echo "protobuf not installed"

cd $repo_root/third_party/protobuf/
make distclean
cd $PROTOBUF_BUILD_DIR

$repo_root/third_party/protobuf/configure --host=$COMPILER --build=i686-pc-linux-gnu --with-protoc=protoc --disable-shared --prefix=$PROTOBUF_BUILD_DIR
make
make install
fi


mkdir -p $WIN32_BUILD_DIR/shm
cd $WIN32_BUILD_DIR/shm

pwd

JAVA_HOME=$J_HOME cmake -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DPROTOBUF_INCLUDE_DIR=$PROTOBUF_BUILD_DIR/include -DPROTOBUF_LIBRARY=$PROTOBUF_BUILD_DIR/lib/libprotobuf.a ../../../

cd $repo_root
pwd
}

echo Linux64
LINUX64_BUILD=builds/linux64/shm
mkdir -p $LINUX64_BUILD
cd $LINUX64_BUILD
cmake ../../../
cd ../../../

echo WIN32
prepareMingw win32 i686-w64-mingw32 ../../../third_party/cmake-toolchains/Toolchain-Ubuntu-mingw32.cmake shmfmi-server/src/jni/java-win32

echo WIN64
prepareMingw win64 x86_64-w64-mingw32 ../../../third_party/cmake-toolchains/Toolchain-Ubuntu-mingw64.cmake shmfmi-server/src/jni/java-win64

