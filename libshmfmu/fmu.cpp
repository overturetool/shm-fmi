/*
 * fmi.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: parallels
 */

#include<stdio.h>

/*
 * fmi2Functions.c
 *
 *  Created on: May 22, 2015
 *      Author: kel
 */

#include "fmi2Functions.h"

#include <string.h>

//#include <map>
#include "FmuProxy.h"

//static std::map <int, FmuProxy> g_clients;
#include <vector>
#include "FmuContainer.h"
#include <sstream>
#include "JavaLauncher.h"
#include "ConfigFile.h"

std::vector<FmuContainer*> g_clients;

static FmuContainer* getFmuContainer(fmi2Component c)
{
	intptr_t index = (intptr_t) c;

	if (g_clients.size() > index)
	{
		return g_clients.at(index);
	}
	return NULL;
}

static fmi2Status convertStatus(FmuProxy::fmi2Status status)
{
	switch (status)
	{
	case FmuProxy::fmi2Status::fmi2Discard:
		return fmi2Discard;
	case FmuProxy::fmi2Status::fmi2Error:
		return fmi2Error;
	case FmuProxy::fmi2Status::fmi2Fatal:
		return fmi2Fatal;
	case FmuProxy::fmi2Status::fmi2OK:
		return fmi2OK;
	case FmuProxy::fmi2Status::fmi2Pending:
		return fmi2Pending;
	case FmuProxy::fmi2Status::fmi2Warning:
		return fmi2Warning;
	}
	return fmi2Error;
}

static FmuProxy::fmi2StatusKind convertStatusKind(fmi2StatusKind kind)
{
	switch (kind)
	{
	case fmi2DoStepStatus:
		return FmuProxy::fmi2StatusKind::fmi2DoStepStatus;
	case fmi2LastSuccessfulTime:
		return FmuProxy::fmi2StatusKind::fmi2LastSuccessfulTime;
	case fmi2PendingStatus:
		return FmuProxy::fmi2StatusKind::fmi2PendingStatus;
	case fmi2Terminated:
		return FmuProxy::fmi2StatusKind::fmi2Terminated;

	}

}

static void log(const fmi2CallbackFunctions *functions, fmi2ComponentEnvironment componentEnvironment,
		fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message)
{
	if (functions != NULL && functions->logger != NULL)
	{
		functions->logger(componentEnvironment, instanceName, status, category, message);
	}
}

static void notimplemented(fmi2Component c, fmi2String message)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		std::string base("Not implemented: ");
		std::string m(message);
		log(fmu->m_functions, NULL, fmu->m_name->c_str(), fmi2Error, "error", (base + m).c_str());
	}
}

void sleepcp(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif // win32
}

void callback(FmuContainer *container, std::string shmCallbackKey)
{
	if (container->logger == NULL)
	{
		return;
	}

	FmiIpc::Client* callbackClient = NULL;
	bool success = false;
	while (container->active && !success)
	{
		callbackClient = new FmiIpc::Client(shmCallbackKey.c_str(), &success);

		if (!success)
		{
			delete callbackClient;
			callbackClient = NULL;
			printf("Unable to connect to callback shm: %s retry in 1 sec.\n\n", shmCallbackKey.c_str());
			sleepcp(1000);
		}

	}

	printf("callback thread connected\n");fflush(stdout);

	if (callbackClient != NULL)
	{
		while (container->active)
		{
			SharedFmiMessage* msg = callbackClient->getMessage(INFINITE);
			printf("callback thread got message\n");fflush(stdout);

			if (msg == NULL)
			{
				printf("callback thread got message ==null, returning, stopping thread\n");fflush(stdout);
				return;
			}
			printf("callback thread got message !=null\n");fflush(stdout);

			if (msg->cmd == sharedfmimemory::fmi2Log)
			{
				Fmi2LogReply* r = new Fmi2LogReply();
				printf("protoBufMsgSize %d\n",msg->protoBufMsgSize);
				for(int j = 0; j < msg->protoBufMsgSize; j++)
				    printf( "%02X", msg->protoBufMsg[j]);
				printf("\n");fflush(stdout);
				r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

				//handle message
				printf("Received log data from client '%s': '%s'\n", container->m_name->c_str(), r->value().c_str());

				fmi2Status status = fmi2Status::fmi2Error;

				switch (r->status())
				{
				case Fmi2LogReply_Status_Ok:
					status = fmi2Status::fmi2OK;
					break;
				case Fmi2LogReply_Status_Warning:
					status = fmi2Status::fmi2Warning;
					break;
				case Fmi2LogReply_Status_Discard:
					status = fmi2Status::fmi2Discard;
					break;
				case Fmi2LogReply_Status_Error:
					status = fmi2Status::fmi2Error;
					break;
				case Fmi2LogReply_Status_Fatal:
					status = fmi2Status::fmi2Fatal;
					break;
				case Fmi2LogReply_Status_Pending:
					status = fmi2Status::fmi2Pending;
					break;
				default:
					status = fmi2Status::fmi2Error;
					break;
				}

				container->logger(container->componentEnvironment, container->m_name->c_str(), status,
						r->category().c_str(), r->value().c_str());

				SharedFmiMessage* msgReply = new SharedFmiMessage();

				Fmi2Empty request;

				msgReply->protoBufMsgSize = request.ByteSize();
				request.SerializeWithCachedSizesToArray(msgReply->protoBufMsg);
				msgReply->cmd = sharedfmimemory::fmi2Log;
				printf("sending callback confirm reply\n");
				callbackClient->sendReply(msgReply);
			}
		}
	}
	printf("callback thread exit\n");fflush(stdout);

}

#define LOG(functions,comp,name,status,category, message,args...) \
	if(functions!=NULL){ \
if(functions->logger!=NULL) \
{ \
	functions->logger((void*)comp , name, status, category, message, args);\
}\
}else {\
	fprintf (stderr, "Name '%s', Category: '%s'\n",name,category);\
	fprintf (stderr, message,args);\
	fprintf (stderr, "\n");\
}

int fmuInternalDebugPrint(void* sender, const char * format, ...)
{
	printf("fmuInternalDebugPrint called...\n");
	va_list args;
	va_start(args, format);
	for (int i = 0; i < g_clients.size() - 1; i++)
	{
		FmuContainer* c = g_clients.at(i);
		if (c->m_proxy->getChannel() == sender)
		{

			// g_clients.at(i)->logger(c->componentEnvironment, c->m_name->c_str(),fmi2OK, "LogAll", format, args);
			va_end(args);
			return 1;
		}
	}

	int ret = vfprintf(stdout, format, args);
	va_end(args);
	return ret;
}

// ---------------------------------------------------------------------------
// FMI functions
// ---------------------------------------------------------------------------
extern "C" fmi2Component fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID,
		fmi2String fmuResourceLocation, const fmi2CallbackFunctions *functions, fmi2Boolean visible,
		fmi2Boolean loggingOn)
{
	//do not return null
	if (g_clients.size() == 0)
	{
		g_clients.push_back(NULL); //Dummy var
	}

	intptr_t compid = g_clients.size();
	if (loggingOn)
	{
		LOG(functions, compid, instanceName, fmi2OK, "logFmiCall",
				"FMU: Called instantiate with instance %s and GUID %s", instanceName, fmuGUID);
	}
	std::string shared_memory_key(fmuGUID);
	shared_memory_key.append(instanceName);

//	setbuf(stdout, NULL); //fixme remove

	std::string resourceLocationStr(fmuResourceLocation);

#ifdef _WIN32
	resourceLocationStr = resourceLocationStr.substr(6);
#elif __APPLE__ ||  __linux
	resourceLocationStr = resourceLocationStr.substr(5);
#endif

	if (loggingOn)
	{
		LOG(functions, compid, instanceName, fmi2OK, "logFmiCall",
				"FMU: Launching Tool Wrapper memory key: '%s'  and resource location %s", shared_memory_key.c_str(),
				resourceLocationStr.c_str());
	}

	std::string configFile = resourceLocationStr + std::string("/config.txt");
	ConfigFile config(configFile, shared_memory_key);

	JavaLauncher::debug = false;
	JavaLauncher *launcher = new JavaLauncher(resourceLocationStr.c_str(), config.m_args);

	if (config.m_skipLaunch)
	{
		LOG(functions, compid, instanceName, fmi2OK, "logFmiCall",
				"FMU: FMU Debug skipping launch of external FMU. SHM is '%s'", shared_memory_key.c_str());
		shared_memory_key = "shmFmiTest";
	}

	if (loggingOn)
	{
		LOG(functions, compid, instanceName, fmi2OK, "logFmiCall", "FMU: Launching with shared memory key: '%s'",
				shared_memory_key.c_str());
	}

	FmuProxy *client = new FmuProxy(shared_memory_key);

	FmiIpc::debug = false;

	FmuContainer *container = new FmuContainer(client, instanceName, functions, launcher);

	g_clients.push_back(container);

	if (functions != NULL && functions->logger != NULL)
	{
		container->logger = functions->logger;
	}

	if (!client->initialize())
	{
		LOG(functions, compid, instanceName, fmi2Fatal, "logFmiCall",
				"FMU: FMU Debug FATAL: cannot create and initialize IPC server for FMU. SHM is '%s'\n",
				shared_memory_key.c_str());
		return NULL;
	}

	if (loggingOn)
	{
		LOG(functions, compid, instanceName, fmi2OK, "logFmiCall",
				"FMU: FMU Server create, hosting SHM with raw key: %s", shared_memory_key.c_str());
	}

//	fflush(stdout); //FIXME remove

	container->componentEnvironment = (void*) compid;

	if (!config.m_skipLaunch)
	{
		launcher->launch();
	}

	std::string callbackId = shared_memory_key + std::string("Callback");
	if (client->fmi2Instantiate(instanceName, fmuGUID, fmuResourceLocation, callbackId.c_str(), visible, loggingOn))
	{
		//connected
		if (functions != NULL && functions->logger != NULL && loggingOn)
		{
			//configure callback
			std::thread* callbackThread = new std::thread(callback, container, callbackId);
		}
	}

	return container->componentEnvironment;
}

extern "C" fmi2Status fmi2SetupExperiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance,
		fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(
				fmu->m_proxy->fmi2SetupExperiment(toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime));
	}

	return fmi2OK;
}

extern "C" fmi2Status fmi2EnterInitializationMode(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2EnterInitializationMode());
	}

	return fmi2Fatal;
}

extern "C" fmi2Status fmi2ExitInitializationMode(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2ExitInitializationMode());
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2Terminate(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2Terminate());
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2Reset(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2Reset());
	}
	return fmi2Fatal;
}

extern "C" void fmi2FreeInstance(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		fmu->active = false;
		fmu->m_javaLauncher->terminate();
		intptr_t index = (intptr_t) c;
		g_clients.at(index) = NULL;
		delete fmu;

	}
}

// ---------------------------------------------------------------------------
// FMI functions: class methods not depending of a specific model instance
// ---------------------------------------------------------------------------

extern "C" const char* fmi2GetVersion()
{
	return fmi2Version;
}

extern "C" const char* fmi2GetTypesPlatform()
{
	return fmi2TypesPlatform;
}

// ---------------------------------------------------------------------------
// FMI functions: logging control, setters and getters for Real, Integer,
// Boolean, String
// ---------------------------------------------------------------------------

extern "C" fmi2Status fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories,
		const fmi2String categories[])
{

	for (int i = 0; i < nCategories; i++)
	{
		if (strcmp(categories[i], "logShm") == 0)
		{
			if (loggingOn)
			{
				FmiIpc::debugPrintPtr = &fmuInternalDebugPrint;
				FmiIpc::debug = true;
			}
		}else if (strcmp(categories[i], "logLaunch") == 0)
		{
			if (loggingOn)
			{
				FmiIpc::debugPrintPtr = &fmuInternalDebugPrint;
				FmiIpc::debug = true;
			}
		}
	}

	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2SetDebugLogging(loggingOn, nCategories, categories));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetReal(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetInteger(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetBoolean(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetString(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2SetReal(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
		const fmi2Integer value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2SetInteger(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
		const fmi2Boolean value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2SetBoolean(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
		const fmi2String value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2SetString(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* FMUstate)
{
	notimplemented(c, "fmi2GetFMUstate");
	return fmi2Error;
}
extern "C" fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate FMUstate)
{
	notimplemented(c, "fmi2SetFMUstate");
	return fmi2Error;
}
extern "C" fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* FMUstate)
{
	notimplemented(c, "fmi2FreeFMUstate");
	return fmi2Error;
}
extern "C" fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate FMUstate, size_t *size)
{
	notimplemented(c, "fmi2SerializedFMUstateSize");
	return fmi2Error;
}
extern "C" fmi2Status fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate FMUstate, fmi2Byte serializedState[],
		size_t size)
{
	notimplemented(c, "fmi2SerializeFMUstate");
	return fmi2Error;
}
extern "C" fmi2Status fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serializedState[], size_t size,
		fmi2FMUstate* FMUstate)
{
	notimplemented(c, "fmi2DeSerializeFMUstate");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetDirectionalDerivative(fmi2Component c, const fmi2ValueReference vUnknown_ref[],
		size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown, const fmi2Real dvKnown[],
		fmi2Real dvUnknown[])
{
	notimplemented(c, "fmi2GetDirectionalDerivative");
	return fmi2Error;
}

// ---------------------------------------------------------------------------
// Functions for FMI for Co-Simulation
// ---------------------------------------------------------------------------
#ifdef FMI_COSIMULATION
/* Simulating the slave */
extern "C" fmi2Status fmi2SetRealInputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
		const fmi2Integer order[], const fmi2Real value[])
{
	notimplemented(c, "fmi2SetRealInputDerivatives");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
		const fmi2Integer order[], fmi2Real value[])
{
	notimplemented(c, "fmi2GetRealOutputDerivatives");
	return fmi2Error;
}

extern "C" fmi2Status fmi2CancelStep(fmi2Component c)
{
	notimplemented(c, "fmi2CancelStep");
	return fmi2Error;
}

extern "C" fmi2Status fmi2DoStep(fmi2Component c, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(
				fmu->m_proxy->fmi2DoStep(currentCommunicationPoint, communicationStepSize,
						noSetFMUStatePriorToCurrentPoint));
	}

	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status *value)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		FmuProxy::fmi2Status status;
		FmuProxy::fmi2StatusKind kind = convertStatusKind(s);
		fmi2Status s = convertStatus(fmu->m_proxy->fmi2GetStatus(kind, &status));
		*value = convertStatus(status);
		return s;
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real *value)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetRealStatus(convertStatusKind(s), value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer *value)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetIntegerStatus(convertStatusKind(s), value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean *value)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetBooleanStatus(convertStatusKind(s), value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String *value)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetStringStatus(convertStatusKind(s), value));
	}
	return fmi2Fatal;
}

/* INTO cps specific*/
extern "C" fmi2Status fmi2GetMaxStepsize(fmi2Component c, fmi2Real* size)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_proxy->fmi2GetMaxStepsize(size));
	}

	return fmi2Fatal;
}

// ---------------------------------------------------------------------------
// Functions for FMI2 for Model Exchange
// ---------------------------------------------------------------------------
#else
/* Enter and exit the different modes */
fmi2Status fmi2EnterEventMode(fmi2Component c)
{
	return fmi2Error;
}

fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *eventInfo)
{
	return fmi2Error;
}

fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
	return fmi2Error;
}

fmi2Status fmi2CompletedIntegratorStep(fmi2Component c, fmi2Boolean noSetFMUStatePriorToCurrentPoint,
		fmi2Boolean *enterEventMode, fmi2Boolean *terminateSimulation)
{
	return fmi2Error;
}

/* Providing independent variables and re-initialization of caching */
fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time)
{
	return fmi2Error;
}

fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[], size_t nx)
{
	return fmi2Error;
}

/* Evaluation of the model equations */
fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[], size_t nx)
{
	return fmi2Error;
}

fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni)
{
	return fmi2Error;
}

fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real states[], size_t nx)
{
	return fmi2Error;
}

fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c, fmi2Real x_nominal[], size_t nx)
{
	return fmi2Error;
}
#endif // Model Exchange
