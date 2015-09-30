

CC=g++

# Define a variable for classpath
CLASS_PATH = ../../target/classes

PROTO_ROOT=C:/msys64/home/kel/grpc-fmi/proto

# Define a virtual path for .class in the bin directory
vpath %.class $(CLASS_PATH)

PATH=C:/data/compiler/mingw32/bin:$PATH

all : sharedmemory.dll
	

# $@ matches the target, $< matches the first dependancy
sharedmemory.dll : sharedmemory.o
	$(CC) -m32 -Wl,--add-stdcall-alias -shared -o $@ $< ../../../libshmfmi/libshmfmi.a -L"C:\msys64\usr\32bit\lib" -lprotobuf
#-Wl,--add-stdcall-alias
# $@ matches the target, $< matches the first dependancy
sharedmemory.o : sharedmemory.cpp $(CLASS_PATH)/org_intocps_java_fmi_shm_SharedMemory.h
	$(CC) -m32 -std=c++0x -I$(CLASS_PATH) -I../../../libshmfmi/ -I"C:/Program Files (x86)/Java/jdk1.7.0_79/include/" -I"C:/Program Files (x86)/Java/jdk1.7.0_79/include/win32" -c $< -o $@

# $* matches the target filename without the extension
$(CLASS_PATH)/org_intocps_java_fmi_shm_SharedMemory.h :$(CLASS_PATH)/org/intocps/java/fmi/shm/SharedMemory.class
	"C:/Program Files (x86)/Java/jdk1.7.0_79/bin/javah" -classpath $(CLASS_PATH) org.intocps.java.fmi.shm.SharedMemory
	
	
	
java:
#	protoc --java_out=src --proto_path=$PROTO_ROOT $PROTO_ROOT/service.proto
	C:/msys64/usr/32bit/bin/protoc --java_out=src --proto_path=~/grpc-fmi/protoc ~/grpc-fmi/proto/service.proto
clean :
	rm $(CLASS_PATH)/org_intocps_java_fmi_shm_SharedMemory.h sharedmemory.o sharedmemory.dll