all:
	make -C libgrpcfmi
	make -C libgrpcfmu
	cd grpcfmi-server && mvn package

test:
	make test -C testlibgrpcfmi
	make test -C testlibgrpcfmu

clean:
	make clean -C libgrpcfmi
	make clean -C libgrpcfmu
	make clean -C testlibgrpcfmi
	make clean -C testlibgrpcfmu
	cd grpcfmi-server && mvn clean
