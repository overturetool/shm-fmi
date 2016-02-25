#!/bin/bash
set -e


threads=4
#SHM_DEPENDENCIES_ROOT=/home/kel/shm2/tmp/

if [ -z "$SHM_DEPENDENCIES_ROOT" ]; 
then echo "The dependency path 'SHM_DEPENDENCIES_ROOT' is now set. Please set it using 'export SHM_DEPENDENCIES_ROOT=/some/path/to/dependency/root'"; exit 1 
		 #else echo "var is set to '$var'"; 
fi

echo "Building with dependencies from: '${SHM_DEPENDENCIES_ROOT}' using make -j${threads}"

function compileDarwin64
{

		echo Building Darwin 64 .dylib
		B=$1/build/darwin64

		rm -rf $B
		mkdir -p $B

		JAVA_HOME=../third_party/jvm/darwin64 cmake  \
						 -B$B \
						 -H$1 \
						 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/osx-gcc.cmake` \
						 -DOSXCROSS_ROOT=$OSXCROSS_ROOT \
						 -DPROTOBUF_PROTOC_EXECUTABLE=/usr/bin/protoc \
						 -DPROTOBUF_INCLUDE_DIRS=$SHM_DEPENDENCIES_ROOT/darwin64/usr/protobuf/include/ \
						 -DPROTOBUF_LIBRARY=$SHM_DEPENDENCIES_ROOT/darwin64/usr/protobuf/lib/libprotobuf.a

		make -C $B -j$threads

}

function compileWin32
{

		echo Building Win32 .dll
		B=$1/build/win32

		rm -rf $B
		mkdir -p $B

		JAVA_HOME=../third_party/jvm/win32 cmake  \
						 -B$B \
						 -H$1 \
						 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/cmake-toolchains/Toolchain-Ubuntu-mingw32.cmake` \
						 -DPROTOBUF_INCLUDE_DIRS=$SHM_DEPENDENCIES_ROOT/win32/usr/protobuf/include/ \
             -DPROTOBUF_LIBRARY=$SHM_DEPENDENCIES_ROOT/win32/usr/protobuf/lib/libprotobuf.a

		# Do to a bug in gtest when compiling with MinGW we have to avoid building test code
		#               make -C $B -j$threads
		make -j$threads -C $B/libshmfmu
		make -j$threads -C $B/shmfmi-server

}

function compileWin64
{

		echo Building Win64 .dll
		B=$1/build/win64

		rm -rf $B
		mkdir -p $B

		JAVA_HOME=../third_party/jvm/win64 cmake  \
						 -B$B \
						 -H$1 \
						 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/cmake-toolchains/Toolchain-Ubuntu-mingw64.cmake` \
						 -DPROTOBUF_INCLUDE_DIRS=$SHM_DEPENDENCIES_ROOT/win64/usr/protobuf/include/ \
             -DPROTOBUF_LIBRARY=$SHM_DEPENDENCIES_ROOT/win64/usr/protobuf/lib/libprotobuf.a 

		# Do to a bug in gtest when compiling with MinGW we have to avoid building test code
		#		make -C $B -j$threads
		make -j$threads -C $B/libshmfmu
		make -j$threads -C $B/shmfmi-server


}

function compileLinux64
{
		echo Building Linux x64 .so
		B=$1/build/linux64
		rm -rf $B
		mkdir -p $B

		JAVA_HOME=../third_party/jvm/linux64 cmake  \
						 -B$B \
						 -H$1 \
						 -DPROTOBUF_INCLUDE_DIRS=$SHM_DEPENDENCIES_ROOT/linux64/usr/protobuf/include/ \
             -DPROTOBUF_LIBRARY=$SHM_DEPENDENCIES_ROOT/linux64/usr/protobuf/lib/libprotobuf.a

		make -C $B -j$threads
}

function compileLinux32
{
		echo Building Linux x32 .so
		B=$1/build/linux32

		rm -rf $B
		mkdir -p $B

		CC="gcc -m32" CXX="g++ -m32" JAVA_HOME=../third_party/jvm/linux64 cmake \
			-B$B \
			-H$1 \
			-DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/linux32-gcc.cmake` \
			-DPROTOBUF_INCLUDE_DIRS=$SHM_DEPENDENCIES_ROOT/linux32/usr/protobuf/include/ \
      -DPROTOBUF_LIBRARY=$SHM_DEPENDENCIES_ROOT/linux32/usr/protobuf/lib/libprotobuf.a

		make -C $B -j$threads
}

function generateJava
{
		OUT=shmfmi-server/target/generated-protoc/
		mkdir -p $OUT
		protoc --java_out=$OUT --proto_path=proto proto/service.proto

}

function createGitInfo
{
		echo Storing git info in 'git-info.txt'

		INFO=git-info.txt
		rm -f $INFO

		git remote -v > $INFO
		git show-ref HEAD >> $INFO
		git rev-parse HEAD >> $INFO

}

function xcompile
{
		D=$1
		echo Compiling using CMake and make
		compileDarwin64 $D
		compileLinux64 $D
		compileLinux32 $D
		compileWin64 $D
		compileWin32 $D
}


createGitInfo

xcompile .

generateJava
