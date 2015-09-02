all:
	make -C libgrpcfmi
	make -C libgrpcfmu
	cd grpcfmi-server && mvn package

test:
	make test -C testlibgrpcfmi
	make test -C testlibgrpcfmu
