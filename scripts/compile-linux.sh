#!/bin/bash

executer=$1
platform=$2

if [ ! -f "$executer" ]; then
echo "missing executer"
exit 1
else
	echo "Compiling with: " $executer

fi


cd $(git rev-parse --show-toplevel)

if [ ! -d "third_party/protobuf/builds/linux-${platform}/release/" ]; then

echo Compiling protobuf dependency

cd third_party/protobuf

../../$executer cmake -B"builds/linux-${platform}" -Hcmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="builds/linux-${platform}/release" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"

../../$executer make -C"builds/linux-${platform}" install


cd ../../
fi


echo Compiling main application

cd $(git rev-parse --show-toplevel)

if [ ! -d "builds" ]; then
	mkdir builds
fi


./$executer cmake   -B"builds/linux-${platform}" -H. -DPYTHON_EXECUTABLE=/usr/bin/python -DCMAKE_INSTALL_PREFIX="/work/third_party/protobuf/builds/linux-${platform}/release/" -DJAVA_HOME=/work/third_party/jvm/linux64

./$executer make   -C"builds/linux-${platform}"

./$executer make   -C"builds/linux-${platform}" test

./$executer bash -c "find \"builds/linux-${platform}\" -name \"*.so\" -exec echo {} \; -exec file {} \; -exec ldd {} \;"
