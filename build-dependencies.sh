#!/bin/bash

FORCE="0"
PLATFORM="linux64"

for i in "$@"
do
		case $i in
				-p=*|--platform=*)
						PLATFORM="${i#*=}"
						shift # past argument=value
						;;
				-f=*|--force=*)
						FORCE=1
						shift # past argument=value
						;;
				--default)
						DEFAULT=YES
						shift # past argument with no value
						;;
				*)
            # unknown option
						;;
		esac
done

set -e

REQUIRED_PACKAGES=("gcc-multilib" "g++-multilib" "curl" "autoconf" "libtool" "automake" "build-essential" "make" "wget")
JOBS=-j4

function checkPkg
{
		set +e
		if [ $(dpkg-query -W -f='${Status}' $1 2>/dev/null | grep -c "ok installed") -eq 0 ];
		then
				set -e
				echo Missing package $1
				echo To install it run:  "'sudo apt-get install ${1}'"
				exit 1
		fi
		set -e
}

function check
{
		#sudo apt-get install gcc-multilib g++-multilib
		for i in "${REQUIRED_PACKAGES[@]}"
		do
				#	echo $i
				checkPkg $i
		done
}



function buildProtobuf
{

		ROOT=$1
		echo "###  Building with root: $ROOT ###"
		mkdir -p "${ROOT}/usr/protobuf"
		PROTOBUF_INSTALL=`readlink -f "${ROOT}/usr/protobuf"`
		
		if [ "$FORCE" = "1" ]
		then
				rm -rf $ROOT
		else

				read -n 1 -p "Remove old build if pressend at $ROOT? (y/n)? " answer
				case ${answer:0:1} in
						y|Y )
								echo Yes
								echo Removing old folder
								rm -rf $ROOT
								;;
						* )
								echo No
								;;
				esac
		fi

		mkdir -p $ROOT
		cd $ROOT
		git clone https://github.com/google/protobuf.git protobuf
		cd protobuf
		git checkout 5d4a856

		# fix for missing download
		wget https://github.com/google/googletest/archive/release-1.7.0.zip
		unzip -q release-1.7.0.zip
		mv googletest-release-1.7.0 gtest
		# end fix
		./autogen.sh

		if [ "$ROOT" == "darwin64" ]; then

				CC=x86_64-apple-darwin15-gcc CXX=x86_64-apple-darwin15-g++ CXXFLAGS=-fPIC ./configure \
					--disable-shared \
					--enable-static \
					--host=x86_64-apple-darwin15 \
					--prefix=$PROTOBUF_INSTALL \
					--with-protoc=protoc

		else

				if [[ $ROOT == *"win"* ]]; then

						if [[ $ROOT == *"64"* ]]; then
								COMPILER=x86_64-w64-mingw32
						else
								COMPILER=i686-w64-mingw32
						fi

						./configure \
								--host=$COMPILER \
								--build=i686-pc-linux-gnu \
								--with-protoc=protoc \
								--disable-shared \
								--prefix=$PROTOBUF_INSTALL

				else

						if [[ $ROOT == *"64"* ]]; then 
								CC_FLAGS=
								CXX_FLAGS=
								LF_FLAGS=
						else
								CC_FLAGS=-m32
								CXX_FLAGS=-m32
								LF_FLAGS=-m32
						fi

						./configure \
								--build=i686-pc-linux-gnu \
								--with-protoc=protoc \
								--disable-shared \
								--prefix=$PROTOBUF_INSTALL  \
								"CCFLAGS=$CC_FLAGS -fPIC" \
								"CXXFLAGS=$CXX_FLAGS -fPIC" \
								"LDFLAGS=$LD_FLAGS"
				fi

		fi

		make $JOBS
		make install

		echo Protobuf is installed to: $PROTOBUF_INSTALL
}
check

if [ "$PLATFORM" = "all" ]
then
		buildProtobuf "linux64"
		buildProtobuf "linux32"
		buildProtobuf "darwin64"
		buildProtobuf "win32"
		buildProtobuf "win64"
else
		buildProtobuf $PLATFORM
fi
