#include "gtest/gtest.h"

#include "ExternalClient.h"
#include <string>
#include <iostream>

/*TEST(ExternalClient, system)
 {
 std::eexec("java -cp .:/home/parallels/Downloads/grpc-fmi/java/bin/:/home/parallels/Downloads/grpc-fmi/java/lib/netty-all-4.1.0.Beta5.jar:/home/parallels/Downloads/grpc-fmi/java/lib/grpc-all-0.8.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/guava-18.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/hpack-0.10.1.jar:/home/parallels/Downloads/grpc-fmi/java/lib/protobuf-java-3.0.0-alpha-3.1.jar rpc.Server");
 }*/

TEST(ExternalClient, fmi2Instantiate)
{
	ExternalClient client("localhost:8980");

	std::string instanceName = "my instance";
	std::string fmuGUID = "dcnkjvnrevirehvljkfnvf";
	std::string fmuResourceLocation = "/tmp";
	const char* callbackAddress = "localhost";
	int callbackPort=8080;
	bool visible = true;
	bool loggingOn = true;

	EXPECT_EQ(true,
			client.fmi2Instantiate(instanceName.c_str(), fmuGUID.c_str(),
					fmuResourceLocation.c_str(), callbackAddress, callbackPort,
					visible, loggingOn));

}

TEST(ExternalClient, fmi2EnterInitializationMode)
{
	ExternalClient client("localhost:8980");
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2EnterInitializationMode());

}

TEST(ExternalClient, fmi2ExitInitializationMode)
{
	ExternalClient client("localhost:8980");
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2ExitInitializationMode());

}

TEST(ExternalClient, fmi2SetupExperiment)
{
	ExternalClient client("localhost:8980");

	bool toleranceDefined = true;
	double tolerance = 0.1;
	double startTime = 0.0;
	bool stopTimeDefined = true;
	double stopTime = 1.1;

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2SetupExperiment(toleranceDefined, tolerance, startTime,
					stopTimeDefined, stopTime));

}

TEST(ExternalClient, fmi2Terminate)
{
	ExternalClient client("localhost:8980");

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2Terminate());
}

TEST(ExternalClient, fmi2SetDebugLogging)
{
	ExternalClient client("localhost:8980");

	int loggingOn = true;
	size_t nCategories = 3;
	const char* categories[3] =
	{ "all", "all+1", "all+2" };

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2SetDebugLogging(loggingOn, nCategories, categories));
}

TEST(ExternalClient, SetGetReals)
{
	ExternalClient client("localhost:8980");

	unsigned int vr[] =
	{ 1, 2, 3 };
	double vals[] =
	{ 1.1, 2.2, 3.3 };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetReal(vr, size, vals));

	double res[size];

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetReal(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}

}

TEST(ExternalClient, SetGetBools)
{
	ExternalClient client("localhost:8980");

	unsigned int vr[] =
	{ 1, 2, 3 };
	int vals[] =
	{ true, false, true };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetBoolean(vr, size, vals));

	int res[size];

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetBoolean(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}

}

TEST(ExternalClient, SetGetIntegers)
{
	ExternalClient client("localhost:8980");

	unsigned int vr[] =
	{ 1, 2, 3 };
	int vals[] =
	{ true, false, true };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetInteger(vr, size, vals));

	int res[size];

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetInteger(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}

}

TEST(ExternalClient, SetGetStrings)
{
	ExternalClient client("localhost:8980");

	unsigned int vr[] =
	{ 1, 2, 3 };

	std::string vals[] =
	{ "Test string: a", "Test string: b", "Test string: c" };

	int size = 3;

	const char* svals[size];

	for (int i = 0; i < size; i++)
	{
		svals[i] = vals[i].c_str();
	}

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetString(vr, size, svals));

	const char* res[size];

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetString(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		ASSERT_STREQ(svals[i], res[i]);
	}

}

TEST(ExternalClient, fmi2DoStep)
{
	ExternalClient client("localhost:8980");

	double currentCommunicationPoint = 0.0;
	double communicationStepSize = 0.1;
	int noSetFMUStatePriorToCurrentPoint = false;

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2DoStep(currentCommunicationPoint, communicationStepSize,
					noSetFMUStatePriorToCurrentPoint));
}

