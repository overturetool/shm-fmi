

CXX = g++
CPPFLAGS += -I/usr/local/include -Iincludes/fmi -I../libgrpcfmi -pthread -g -fPIC -DFMI_COSIMULATION
CFLAGS += -g -fPIC
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib -Wl,-Bstatic    -L../libgrpcfmi -lgrpcfmi -lprotobuf -lgrpc++ -lgrpc -lgpr  -lgrpc++_unsecure ../third_party/zlib/libz.a -Wl,-Bdynamic -lpthread  -ldl 


#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a -shared
#-lgrpc++_unsecure
#/usr/local/lib/libprotobuf.a
#LDFLAGS += -L/usr/local/lib  /usr/local/lib/libgrpc.a /usr/local/lib/libgpr.a /usr/local/lib/libprotobuf.a -lpthread -ldl /usr/local/lib/libgrpc++_unsecure.a

DEPS = FmuContainer.h ExternalClient.h freeport.h JavaLauncher.h ConfigFile.h


all: libgrpcfmu.so
# libgrpcfmi.a

libgrpcfmu.so: fmu.o FmuContainer.o freeport.o JavaLauncher.o ConfigFile.o ../third_party/zlib/libz.a
	$(CXX) $^ $(LDFLAGS) -shared -o $@

%.o: %.cpp $(DEPS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

#libgrpcfmi.a: service.pb.o service.grpc.pb.o ExternalClient.o
#	ar rvs $@ $^ 

ExternalClient.h :
	make -C ../libgrpcfmi

../third_party/zlib/libz.a :
	cd ../third_party/zlib/; CC="$(CC)" CFLAGS="-fPIC -fvisibility=hidden " ./configure --static
	make -C ../third_party/zlib

clean:
	rm -f *.o *.pb.cc *.pb.h *.so *.a




