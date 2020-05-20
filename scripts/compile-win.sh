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

if [ ! -d "third_party/protobuf/builds/win${platform}/release/" ]; then

echo Compiling protobuf dependency

cd third_party/protobuf

../../$executer cmake -B"builds/win${platform}" -Hcmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="builds/win${platform}/release" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"

../../$executer make -C"builds/win${platform}" install


docker run --rm dockcross/linux-x64:latest >linux_executer
chmod +x linux_executer


./linux_executer cmake -B"builds/linux-x64" -Hcmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="builds/linux-x64/release" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"

./linux_executer make -C"builds/linux-x64" install


cd ../../
fi


echo Compiling main application

cd $(git rev-parse --show-toplevel)

if [ ! -d "builds" ]; then
	mkdir builds
fi


./$executer cmake   -B"builds/win${platform}" -H. \
					-DPYTHON_EXECUTABLE=/usr/bin/python -UCMAKE_SYSTEM_NAME -DCMAKE_SYSTEM_NAME=Windows \
					-DCMAKE_PREFIX_PATH="/work/third_party/protobuf/builds/win${platform}/release/lib/cmake/protobuf" \
					-DCMAKE_INSTALL_PREFIX="/work/third_party/protobuf/builds/win${platform}/release/" \
					-DJAVA_HOME="/work/third_party/jvm/win${platform:1}" \
					-DProtobuf_LIBRARIES="/work/third_party/protobuf/builds/win${platform}/release/lib/libprotobuf.a" \
					-DProtobuf_INCLUDE_DIR="/work/third_party/protobuf/builds/win${platform}/release/include" \
					-DProtobuf_PROTOC_EXECUTABLE=/work/third_party/protobuf/builds/linux-x64/protoc

./$executer make   -C"builds/win${platform}" VERBOSE=1

#./$executer make   -C"builds/win-${platform}" test

#./$executer bash -c "find \"builds/win-${platform}\" -name \"*.so\" -exec echo {} \; -exec file {} \; -exec ldd {} \;"

docker run -it -v $(pwd):/work thawsystems/wine-stable wine /work/builds/win${platform}/testfmu-integration/testfmu-integration.exe

./$executer  bash -c "find /work -name \"*.dll\" -exec echo {} \; -exec bash -c \"objdump -p {} | grep 'DLL Name:'\" \;"
