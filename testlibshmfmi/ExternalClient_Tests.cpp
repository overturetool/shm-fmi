#include "gtest/gtest.h"

#include "ExternalClient.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"

#define MEM_KEY "shmFmiTest"

/*TEST(ExternalClient, system)
 {
 std::eexec("java -cp .:/home/parallels/Downloads/grpc-fmi/java/bin/:/home/parallels/Downloads/grpc-fmi/java/lib/netty-all-4.1.0.Beta5.jar:/home/parallels/Downloads/grpc-fmi/java/lib/grpc-all-0.8.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/guava-18.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/hpack-0.10.1.jar:/home/parallels/Downloads/grpc-fmi/java/lib/protobuf-java-3.0.0-alpha-3.1.jar rpc.Server");
 }*/

void clientThread()
{
	bool success;
	FmiIpc::Client client(MEM_KEY, &success);

	SharedFmiMessage* msg = client.getMessage(0);
	printf("Client got message\n");
	msg->cmd = fmi2DoStep;

	printf("Client is sending back reply now\n");
	client.sendReply(msg);
	printf("Client terminating\n");
	return;
}

TEST(FmiIpc, shmbasetest)
{
	FmiIpc::Server server(MEM_KEY);

	std::thread t1(clientThread);

	printf("Server is sending message to client\n");

	SharedFmiMessage msg;

	msg.cmd = fmi2SetupExperiment;

	server.send(&msg, 0);

	printf("Server got reply from client");

	t1.join();
	EXPECT_EQ(true, true);
}

void remoteClientThread()
{
	remoteTestDriverSingle(MEM_KEY);
}

void remoteClientThreadResume()
{
	remoteTestDriver(MEM_KEY);
}

TEST(ExternalClient, fmi2Instantiate)
{

	ExternalClient client(MEM_KEY);
	std::thread t1(remoteClientThread);

	std::string instanceName = "my instance";
	std::string fmuGUID = "dcnkjvnrevirehvljkfnvf";
	std::string fmuResourceLocation = "/tmp";
	const char* callbackAddress = "localhost";
	int callbackPort = 8080;
	bool visible = true;
	bool loggingOn = true;

	EXPECT_EQ(true,
			client.fmi2Instantiate(instanceName.c_str(), fmuGUID.c_str(),
					fmuResourceLocation.c_str(), callbackAddress, callbackPort,
					visible, loggingOn));
	t1.join();
}

TEST(ExternalClient, fmi2EnterInitializationMode)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2EnterInitializationMode());
	t1.join();
}

TEST(ExternalClient, fmi2ExitInitializationMode)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2ExitInitializationMode());
	t1.join();
}

TEST(ExternalClient, fmi2SetupExperiment)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	bool toleranceDefined = true;
	double tolerance = 0.1;
	double startTime = 0.0;
	bool stopTimeDefined = true;
	double stopTime = 1.1;

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2SetupExperiment(toleranceDefined, tolerance, startTime,
					stopTimeDefined, stopTime));
	t1.join();
}

TEST(ExternalClient, fmi2Terminate)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2Terminate());
	t1.join();
}

TEST(ExternalClient, fmi2SetDebugLogging)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	int loggingOn = true;
	size_t nCategories = 3;
	const char* categories[3] =
	{ "all", "all+1", "all+2" };

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2SetDebugLogging(loggingOn, nCategories, categories));
	t1.join();
}

TEST(ExternalClient, SetGetReals)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	unsigned int vr[] =
	{ 1, 2, 3 };
	double vals[] =
	{ 1.1, 2.2, 3.3 };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetReal(vr, size, vals));
	t1.join();
	double res[size];

	std::thread t2(remoteClientThreadResume);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetReal(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}
	t2.join();
}

TEST(ExternalClient, SetGetBools)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	unsigned int vr[] =
	{ 1, 2, 3 };
	int vals[] =
	{ true, false, true };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetBoolean(vr, size, vals));
	t1.join();
	int res[size];
	std::thread t2(remoteClientThreadResume);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetBoolean(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}
	t2.join();
}

TEST(ExternalClient, SetGetIntegers)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	unsigned int vr[] =
	{ 1, 2, 3 };
	int vals[] =
	{ true, false, true };

	int size = 3;

	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2SetInteger(vr, size, vals));
	t1.join();
	int res[size];
	std::thread t2(remoteClientThreadResume);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetInteger(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		EXPECT_EQ(vals[i], res[i]);
	}
	t2.join();
}

TEST(ExternalClient, SetGetStrings)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

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
	t1.join();
	const char* res[size];
	std::thread t2(remoteClientThreadResume);
	EXPECT_EQ(ExternalClient::fmi2OK, client.fmi2GetString(vr, size, res));

	for (int i = 0; i < size; i++)
	{
		ASSERT_STREQ(svals[i], res[i]);
	}
	t2.join();
}

TEST(ExternalClient, fmi2DoStep)
{
	std::thread t1(remoteClientThread);
	ExternalClient client(MEM_KEY);

	double currentCommunicationPoint = 0.0;
	double communicationStepSize = 0.1;
	int noSetFMUStatePriorToCurrentPoint = false;

	EXPECT_EQ(ExternalClient::fmi2OK,
			client.fmi2DoStep(currentCommunicationPoint, communicationStepSize,
					noSetFMUStatePriorToCurrentPoint));
	t1.join();
}

