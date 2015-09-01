

CXX = g++
CPPFLAGS += -I/usr/local/include -Iincludes/fmi -I../libgrpcfmi -pthread -g -fPIC -DFMI_COSIMULATION
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib  -lgrpc -lgpr -lprotobuf -lgrpc++_unsecure -lpthread -ldl
#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a -shared
#-lgrpc++_unsecure
#/usr/local/lib/libprotobuf.a
#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a

DEPS = FmuContainer.h ExternalClient.h


all: libgrpcfmu.so
# libgrpcfmi.a

libgrpcfmu.so: fmu.o
	$(CXX) $^ $(LDFLAGS) -shared -o $@

%.o: %.cpp $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

#libgrpcfmi.a: service.pb.o service.grpc.pb.o ExternalClient.o
#	ar rvs $@ $^ 

ExternalClient.h :
	make -C ../libgrpcfmi

clean:
	rm -f *.o *.pb.cc *.pb.h *.so *.a




