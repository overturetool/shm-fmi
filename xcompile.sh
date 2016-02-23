#!/bin/bash
set -e

threads=4

function compileDarwin64
{

		echo Building Darwin 64 .dylib
		B=$1/build/darwin64

		rm -rf $B
		mkdir -p $B

		cmake  -B$B -H$1 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/osx-gcc.cmake` -DOSXCROSS_ROOT=$OSXCROSS_ROOT

		make -C $B -j$threads

}

function compileWin32
{

		echo Building Win32 .dll
		B=$1/build/win32

		rm -rf $B
		mkdir -p $B

		JAVA_HOME=shmfmi-server/src/jni/java-win32 cmake  -B$B -H$1 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/cmake-toolchains/Toolchain-Ubuntu-mingw32.cmake`

		make -C $B -j$threads

}

function compileWin64
{

		echo Building Win64 .dll
		B=$1/build/win64

		rm -rf $B
		mkdir -p $B

		JAVA_HOME=shmfmi-server/src/jni/java-win64 cmake  -B$B -H$1 -DCMAKE_TOOLCHAIN_FILE=`readlink -f toolchains/cmake-toolchains/Toolchain-Ubuntu-mingw64.cmake`

		make -C $B -j$threads

}

function compileLinux64
{
		echo Building Linux x64 .so
		B=$1/build/linux64
		rm -rf $B
		mkdir -p $B

		cmake  -B$B -H$1

		make -C $B -j$threads
}

function compileLinux32
{
		echo Building Linux x32 .so
		B=$1/build/linux32

		rm -rf $B
		mkdir -p $B

		cmake -B$B -H$1 -DLINK_FLAGS="-m32" -DCFLAGS="-m32"

		make -C $B -j$threads
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




xcompile .

