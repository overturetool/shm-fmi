#!/bin/sh

echo Creating generation folder
mkdir -p src/main/java

PROTO_ROOT=../../../../proto
OUTPUT=../../../target/generated-protoc/

mkdir -p $OUTPUT

echo Running protoc
protoc --java_out=$OUTPUT --proto_path=$PROTO_ROOT $PROTO_ROOT/service.proto

