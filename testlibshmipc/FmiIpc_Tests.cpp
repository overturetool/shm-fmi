#include "gtest/gtest.h"

#include <string>
#include <iostream>
#include <thread>

#include "IpcBase.h"
#include "IpcServer.h"
#include "IpcClient.h"

void runServer(const char* key, const char* data, SharedFmiMessage*messageSend, SharedFmiMessage* returnMessage,
		bool*running)
{
	FmiIpc::IpcServer* server = new FmiIpc::IpcServer(0, key);
	*running = server->create();

	SharedFmiMessage* message = new SharedFmiMessage();

	message->cmd = fmi2Command::fmi2DoStep;

	int length = strlen(data);

	for (int i = 0; i < length; i++)
	{
		message->protoBufMsg[i] = data[i];
	}

	message->protoBufMsgSize = length;

	*messageSend = *message;
//	printf("Sending %s, with length %i\n", message->protoBufMsg, message->protoBufMsgSize);
	SharedFmiMessage* messageR = server->send(message, 0);
	*returnMessage = *messageR;

	server->close();
	delete server;
}

void runClient(const char* key)
{
	bool success;

	FmiIpc::IpcClient* client = NULL;

	while (!success)
	{
//		printf("Trying to connect and answer. Key: %s\n", key);
		client = new FmiIpc::IpcClient(1, &success, key);
		if (!success)
			delete client;
	}

//	printf("Waiting to receive message\n");
	SharedFmiMessage* message = client->getMessage(0);

	if (message == NULL)
	{
		printf("Did only receive a NULL\n");
		delete client;
		return;
	}

//	printf("Received Cmd: %i, Data: %s, Length: %i\n", message->cmd, message->protoBufMsg, message->protoBufMsgSize);
	client->sendReply(message);
	delete client;
}

TEST(FmiIpc, singleTest)
{
	const char* key = "key";
	const char data[] =
	{ 'a', 'b', 'c', 0 };
	SharedFmiMessage send;
	SharedFmiMessage recieved;
	bool running = false;
	std::thread* callbackThread = new std::thread(runServer, key, data, &send, &recieved, &running);
	while (!running)
		for (int i = 0; i < 1000; i++)
			;
	runClient(key);

	callbackThread->join();

	SharedFmiMessage s = send;
	SharedFmiMessage r = recieved;

	EXPECT_EQ(s.protoBufMsgSize, r.protoBufMsgSize);

	EXPECT_STREQ((const char* )s.protoBufMsg, (const char* )r.protoBufMsg);

	for (int i = 0; i < s.protoBufMsgSize; i++)
	{
		EXPECT_EQ(s.protoBufMsg[i], r.protoBufMsg[i]);
	}
}

