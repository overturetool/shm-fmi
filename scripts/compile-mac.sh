#!/bin/bash

cd $(git rev-parse --show-toplevel)

executer=docker_mac_proto


#docker_mac_proto

if [ ! -f "$executer" ]; then
echo "missing executer"
echo "Lets build it..."
cd dockers/docker_mac_proto
docker build -t docker_mac_proto .
cd ../../
docker run docker_mac_proto > docker_mac_proto
chmod +x docker_mac_proto
echo Executor is now ready
fi

echo "Compiling with: " $executer


cd $(git rev-parse --show-toplevel)

if [ ! -d "third_party/protobuf/builds/mac/release/" ]; then

  echo Compiling protobuf dependency

  cd third_party/protobuf

  ../../$executer cmake -B"builds/mac" -Hcmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="builds/mac/release" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"

  ../../$executer make -C"builds/mac" install -j9

  cd ../../


  # we need this for cross compilation just the executable through
  if [ ! -d "third_party/protobuf/builds/linux-x64/release/" ]; then

    docker run --rm dockcross/linux-x64:latest >linux_executer
    chmod +x linux_executer


    ./linux_executer cmake -B"builds/linux-x64" -Hcmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="builds/linux-x64/release" -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fPIC"

    ./linux_executer make -C"builds/linux-x64" install -j9
  fi


fi


./docker-mac cmake   -Bbuilds/mac  -H.   -DPYTHON_EXECUTABLE=/usr/bin/python   -DPROTOC_EXECUTABLE_OVERRIDE_PATH=/work/third_party/protobuf/builds/linux-x64/release/bin/protoc -DCMAKE_INSTALL_PREFIX=/work/third_party/protobuf/builds/mac/release/ -Dprotobuf_MODULE_COMPATIBLE=1 -DJAVA_HOME=/work/third_party/jvm/darwin64

./docker-mac make   -Cbuilds/mac


./docker-mac bash -c "find builds/mac -name \"*.dylib\" -exec echo {} \; -exec /opt/osxcross/target/bin/x86_64-apple-darwin19-otool -L {} \;"
