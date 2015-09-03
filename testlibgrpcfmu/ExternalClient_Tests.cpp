#include "gtest/gtest.h"


#include <string>
#include <iostream>

extern "C" {
#include "fmu-loader.h"
}
/*TEST(ExternalClient, system)
{
	std::eexec("java -cp .:/home/parallels/Downloads/grpc-fmi/java/bin/:/home/parallels/Downloads/grpc-fmi/java/lib/netty-all-4.1.0.Beta5.jar:/home/parallels/Downloads/grpc-fmi/java/lib/grpc-all-0.8.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/guava-18.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/hpack-0.10.1.jar:/home/parallels/Downloads/grpc-fmi/java/lib/protobuf-java-3.0.0-alpha-3.1.jar rpc.Server");
}*/

TEST(A, fmi2EnterInitializationMode)
{
	FMU fmu;
	//ExternalClient client("localhost:8980");
	//EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2EnterInitializationMode());
	HMODULE h;
int res=	loadDll(FMULIB,&fmu,&h);
EXPECT_EQ(1,res);

int r =fmu.enterInitializationMode(0);

EXPECT_EQ(1,1);

//dlclose(h);
}


TEST(A, instantiate)
{
	FMU fmu;

	HMODULE h;


int res=	loadDll(FMULIB,&fmu,&h);
EXPECT_EQ(1,res);

std::string instanceName = "my instance";
	std::string fmuGUID = "dcnkjvnrevirehvljkfnvf";
	std::string fmuResourceLocation = "/tmp";
	//const char* callbackAddress = "localhost";
	//int callbackPort=8080;
	//bool visible = true;
	//bool loggingOn = true;



	fmi2Component r =fmu.instantiate(instanceName.c_str(),fmi2CoSimulation,fmuGUID.c_str(),fmuResourceLocation.c_str(),NULL,true,true);
if(r==r)
	int k=0;
EXPECT_EQ(1,1);
dlclose(h);
}




