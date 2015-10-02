CXX = g++
CPPFLAGS += -I/usr/local/include -pthread -g -fPIC 
#-Ic:/msys32/usr/local/include
#-Ic:/msys64/usr/32bit/include
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib   -lprotobuf  
#-LC:/msys32/usr/local/lib
#-LC:/msys64/usr/32bit/lib
PROTOC = protoc

PROTOS_PATH = ../proto/

vpath %.proto $(PROTOS_PATH)

all: libshmfmi.a libshmfmi.so

libshmfmi.so: service.pb.o ExternalClient.o FmiIpc.o
	$(CXX) $^ $(LDFLAGS) -shared -o $@

libshmfmi.a: service.pb.o ExternalClient.o FmiIpc.o
	ar rvs $@ $^ 

%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h *.so *.a




#./configure --disable-shared --prefix=/usr/32bit/