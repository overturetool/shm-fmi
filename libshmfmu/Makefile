

CXX = g++
CPPFLAGS += -I/usr/local/include -Iincludes/fmi -I../libshmfmi -pthread -g -fPIC -DFMI_COSIMULATION -Ic:/msys32/usr/local/include
CFLAGS += -g -fPIC
CXXFLAGS += -std=c++11
LDFLAGS += -static-libstdc++  -L/usr/local/lib -LC:/msys32/usr/local/lib  -Wl,-Bstatic    -L../libshmfmi -lshmfmi -lprotobuf -lstdc++   -Wl,-Bdynamic -lpthread -lrt


#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a -shared
#-lgrpc++_unsecure
#/usr/local/lib/libprotobuf.a
#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a

DEPS = FmuContainer.h ExternalClient.h JavaLauncher.h ConfigFile.h


all: libshmfmu.so
# libgrpcfmi.a

libshmfmu.so: fmu.o FmuContainer.o JavaLauncher.o ConfigFile.o 
	$(CXX) $^ $(LDFLAGS) -shared -o $@

%.o: %.cpp $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

#libgrpcfmi.a: service.pb.o service.grpc.pb.o ExternalClient.o
#	ar rvs $@ $^ 

ExternalClient.h :
	make -C ../libshmfmi



clean:
	rm -f *.o *.pb.cc *.pb.h *.so *.a




