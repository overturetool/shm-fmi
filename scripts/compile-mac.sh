#!/bin/bash

cd $(git rev-parse --show-toplevel)

rm -rf builds/mac


./docker-mac cmake   -Bbuilds/mac  -H.   -DPYTHON_EXECUTABLE=/usr/bin/python   -DPROTOC_EXECUTABLE_OVERRIDE_PATH=/work/third_party/protobuf/builds/linux/release/bin/protoc -DCMAKE_INSTALL_PREFIX=/work/third_party/protobuf/builds/mac/release/ -Dprotobuf_MODULE_COMPATIBLE=1 -DJAVA_HOME=/work/third_party/jvm/darwin64

./docker-mac make   -Cbuilds/mac


./docker-mac bash -c 'find builds/linux -name "*.so" -exec echo {} \; -exec ldd {} \;'  
