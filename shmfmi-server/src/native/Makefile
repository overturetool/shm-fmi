

CC=g++

# Define a variable for classpath
CLASS_PATH = ../../target/classes

PROTO_ROOT=C:/msys64/home/kel/grpc-fmi/proto

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

#PATH=C:/data/compiler/mingw32/bin:$PATH

#PATH=C:/msys32/mingw32/bin:$PATH
#JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
#PATH=C:/msys32:$PATH

all : sharedmemory.so


# $@ matches the target, $< matches the first dependancy
sharedmemory.so : sharedmemory.o
	$(CC) -v
	$(CC) -std=c++11 -fPIC  -static-libstdc++    -Wl,-Bstatic -lprotobuf    -Wl,-Bdynamic  -ldl -lpthread -lprotobuf -shared -o $@ $<   ../../../libshmfmi/libshmfmi.a -lrt
	mkdir -p $(CLASS_PATH)/lib/Linux-amd64
	cp sharedmemory.so $(CLASS_PATH)/lib/Linux-amd64/libsharedmemory.so
#-Wl,--add-stdcall-alias
# $@ matches the target, $< matches the first dependancy
sharedmemory.o : sharedmemory.cpp $(CLASS_PATH)/org_intocps_java_fmi_shm_SharedMemory.h
	$(CC)  -std=c++11 -fPIC -I$(CLASS_PATH) -I../../../libshmfmi/ -I/usr/lib/jvm/java-7-openjdk-amd64/include -I/usr/lib/jvm/java-7-openjdk-amd64/include/linux -c $< -o $@

# $* matches the target filename without the extension
$(CLASS_PATH)/org_intocps_java_fmi_shm_SharedMemory.h :$(CLASS_PATH)/org/intocps/java/fmi/shm/SharedMemory.class
	javah -classpath $(CLASS_PATH) org.intocps.java.fmi.shm.SharedMemory
	
	
	
java:
#	protoc --java_out=src --proto_path=$PROTO_ROOT $PROTO_ROOT/service.proto
	C:/msys64/usr/32bit/bin/protoc --java_out=src --proto_path=~/grpc-fmi/protoc ~/grpc-fmi/proto/service.proto
clean :
	rm -f org_intocps_java_fmi_shm_SharedMemory.h org_intocps_java_fmi_shm_SharedMemory_NativeFmuApi.h sharedmemory.o sharedmemory.dll