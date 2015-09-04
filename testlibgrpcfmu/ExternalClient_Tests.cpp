#include "gtest/gtest.h"

#include <string>
#include <iostream>

extern "C"
{
#include "fmu-loader.h"
}
/*TEST(ExternalClient, system)
 {
 std::eexec("java -cp .:/home/parallels/Downloads/grpc-fmi/java/bin/:/home/parallels/Downloads/grpc-fmi/java/lib/netty-all-4.1.0.Beta5.jar:/home/parallels/Downloads/grpc-fmi/java/lib/grpc-all-0.8.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/guava-18.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/hpack-0.10.1.jar:/home/parallels/Downloads/grpc-fmi/java/lib/protobuf-java-3.0.0-alpha-3.1.jar rpc.Server");
 }*/


static FMU s_fmu;
static bool s_libLoaded = false;

/*
TEST(LibraryLoad, loadsingle)
{
	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}

TEST(LibraryLoad, loaddouble)
{
	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}*/

static FMU setup()
{
if(s_libLoaded)
	return s_fmu;

	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));

	s_fmu = fmu;
	s_libLoaded = true;
	return fmu;
}

TEST(FMU, instantiate)
{
	FMU fmu = setup();
	fmu.instantiate("A", fmi2CoSimulation,
			"{348783748923}", ".", NULL, true, true);
}

TEST(FMU, fmi2EnterInitializationMode)
{
	FMU fmu = setup();
	fmi2Component comp = fmu.instantiate("A", fmi2CoSimulation,
			"{348783748923}", ".", NULL, true, true);

	fmi2Status status = fmu.enterInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
}



