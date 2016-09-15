/*
 * RemoveTestDriver.cpp
 *
 *  Created on: Oct 5, 2015
 *      Author: parallels
 */
#include "FmuProxy.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"
#include "IpcClient.h"

#define realIdGet 10
#define intIdGet 11
#define boolIdGet 12
#define stringIdGet 13

#define TD_SIZE 100

static double reals[TD_SIZE] =
{ 0 };

static bool bools[TD_SIZE] =
{ 0 };

static int ints[TD_SIZE] =
{ 0 };

static std::string strings[TD_SIZE];

void okReply(SharedFmiMessage* msg)
{
	Fmi2StatusReply* r = new Fmi2StatusReply();
	r->set_status(Fmi2StatusReply_Status_Ok);
	msg->protoBufMsgSize = r->ByteSize();
	r->SerializeWithCachedSizesToArray(msg->protoBufMsg);
}

static  FmiIpc::IpcClient* globalClient = NULL;

void remoteTestDriverSingle(const char * shmKey)
{
	globalClient = NULL;
	remoteTestDriver(shmKey);
}

void remoteTestDriver(const char * shmKey)
{

	reals[ realIdGet] =9.9;
	ints[ intIdGet ] =1;
	bools[ boolIdGet ] =true;
	strings[ stringIdGet ] =std::string("undefined");


	bool success;

	FmiIpc::IpcClient* client = globalClient;

	if (globalClient == NULL)
	{
	//	printf("Trying to connect client: ");
		while(!success || client==NULL)
		{
	//	printf(".");
			client = new FmiIpc::IpcClient(0, &success,shmKey);
		}
	//	printf("\n Connected.\n");

	}


	SharedFmiMessage* msg = client->getMessage(0);

//	printf("Client got message\n");

	switch (msg->cmd)
	{
	case sharedfmimemory::fmi2FreeInstance:
		delete client;
		exit(0);
		break;

	case sharedfmimemory::fmi2SetDebugLogging:
	case sharedfmimemory::fmi2Instantiate:
		//	case sharedfmimemory::fmi2FreeInstance:
	case sharedfmimemory::fmi2SetupExperiment:
	case sharedfmimemory::fmi2EnterInitializationMode:
	case sharedfmimemory::fmi2ExitInitializationMode:
	case sharedfmimemory::fmi2Terminate:
	case sharedfmimemory::fmi2Reset:
	case sharedfmimemory::fmi2DoStep:
		//case sharedfmimemory::fmi2SetReal:
		//case sharedfmimemory::fmi2SetInteger:
		//case sharedfmimemory::fmi2SetBoolean:
		//case sharedfmimemory::fmi2SetString:
	case sharedfmimemory::fmi2GetStatus:
	{
		okReply(msg);

	}
		break;
	case sharedfmimemory::fmi2SetReal:
	{
		Fmi2SetRealRequest* r = new Fmi2SetRealRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			reals[id] = r->values(i);
		}

		okReply(msg);
	}
		break;
	case sharedfmimemory::fmi2SetInteger:
	{
		Fmi2SetIntegerRequest* r = new Fmi2SetIntegerRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			ints[id] = r->values(i);
		}
		okReply(msg);
	}
		break;
	case sharedfmimemory::fmi2SetBoolean:
	{
		Fmi2SetBooleanRequest* r = new Fmi2SetBooleanRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			bools[id] = r->values(i);
		}
		okReply(msg);
	}
		break;
	case sharedfmimemory::fmi2SetString:
	{
		Fmi2SetStringRequest* r = new Fmi2SetStringRequest();
				r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

				for (int i = 0; i < r->valuereference_size(); i++)
				{
					int id = r->valuereference(i);
					strings[id]= new char[40];
					const char * s = r->values(i).c_str();
					  strings[id]=*new std::string(s);

				}
				okReply(msg);
	}
		break;
	case sharedfmimemory::fmi2GetReal:
	{
		Fmi2GetRequest* r = new Fmi2GetRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2GetRealReply* reply = new Fmi2GetRealReply();

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			reply->add_values(reals[id]);

		}

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetBoolean:
	{
		Fmi2GetRequest* r = new Fmi2GetRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2GetBooleanReply* reply = new Fmi2GetBooleanReply();

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			reply->add_values(bools[id]);

		}

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetInteger:
	{
		Fmi2GetRequest* r = new Fmi2GetRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2GetIntegerReply* reply = new Fmi2GetIntegerReply();

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			reply->add_values(ints[id]);

		}

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetString:
	{
		Fmi2GetRequest* r = new Fmi2GetRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2GetStringReply* reply = new Fmi2GetStringReply();

		for (int i = 0; i < r->valuereference_size(); i++)
		{
			int id = r->valuereference(i);
			//reply->add_values(ints[id]);
			reply->add_values(strings[id]);
		}

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;

	case sharedfmimemory::fmi2GetMaxStepSize:
	{

		Fmi2GetMaxStepSizeReply* reply = new Fmi2GetMaxStepSizeReply();

		reply->set_maxstepsize(100);

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;

	case sharedfmimemory::fmi2GetRealStatus:
	{
		Fmi2StatusRequest* r = new Fmi2StatusRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2RealStatusReply* reply = new Fmi2RealStatusReply();

		if (r->status() == Fmi2StatusRequest::fmi2LastSuccessfulTime)
			reply->set_value(100.5);

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetIntegerStatus:
	{
		Fmi2StatusRequest* r = new Fmi2StatusRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2IntegerStatusReply* reply = new Fmi2IntegerStatusReply();

		if (r->status() == Fmi2StatusRequest::fmi2LastSuccessfulTime)
			reply->set_value(100);

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetBooleanStatus:
	{
		Fmi2StatusRequest* r = new Fmi2StatusRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2BooleanStatusReply* reply = new Fmi2BooleanStatusReply();

		if (r->status() == Fmi2StatusRequest::fmi2Terminated)
			reply->set_value(true);

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	case sharedfmimemory::fmi2GetStringStatus:
	{
		Fmi2StatusRequest* r = new Fmi2StatusRequest();
		r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

		Fmi2StringStatusReply* reply = new Fmi2StringStatusReply();

		if (r->status() == Fmi2StatusRequest::fmi2DoStepStatus)
			reply->set_value("waiting");

		msg->protoBufMsgSize = reply->ByteSize();
		reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
	}
		break;
	default:

		break;
	}
	client->sendReply(msg);

	return;
}
