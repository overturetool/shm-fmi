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
#include "ExternalClient.h"

//static std::map <int, ExternalClient> clients;
#include <vector>
#include "FmuContainer.h"

std::vector<FmuContainer*> clients;

static FmuContainer* getFmuContainer(fmi2Component c)
{
	intptr_t index = (intptr_t) c;

	if (clients.size() > index)
	{
		return clients.at(index);
	}
	return NULL;
}

static fmi2Status convertStatus(ExternalClient::fmi2Status status)
{
	switch (status)
	{
	case ExternalClient::fmi2Status::fmi2Discard:
		return fmi2Discard;
	case ExternalClient::fmi2Status::fmi2Error:
		return fmi2Error;
	case ExternalClient::fmi2Status::fmi2Fatal:
		return fmi2Fatal;
	case ExternalClient::fmi2Status::fmi2OK:
		return fmi2OK;
	case ExternalClient::fmi2Status::fmi2Pending:
		return fmi2Pending;
	case ExternalClient::fmi2Status::fmi2Warning:
		return fmi2Warning;
	}
	return fmi2Error;
}

static void log(const fmi2CallbackFunctions *functions,
		fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message)
{
	if (functions != NULL && functions->logger != NULL)
	{
		functions->logger(componentEnvironment, instanceName, status, category,
				message);
	}
}

static void notimplemented(fmi2Component c, fmi2String message)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		std::string base("Not imeplemented: ");
		std::string m(message);
		log(fmu->m_functions, NULL, fmu->m_name, fmi2Error, "error",
				(base + m).c_str());
	}
}

// ---------------------------------------------------------------------------
// FMI functions
// ---------------------------------------------------------------------------
extern "C" fmi2Component fmi2Instantiate(fmi2String instanceName,
		fmi2Type fmuType, fmi2String fmuGUID, fmi2String fmuResourceLocation,
		const fmi2CallbackFunctions *functions, fmi2Boolean visible,
		fmi2Boolean loggingOn)
{
	//printf("c++ fmi2Instantiate");

	std::string url = "localhost:8980";

	ExternalClient *client = new ExternalClient(url);

	FmuContainer *container = new FmuContainer(client, instanceName, functions);

	clients.push_back(container);

	client->fmi2Instantiate(instanceName, fmuGUID, fmuResourceLocation, "",
			8900, visible, loggingOn);

	intptr_t compid = clients.size() - 1;

	return (void*) compid;
}

extern "C" fmi2Status fmi2SetupExperiment(fmi2Component c,
		fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime,
		fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(
				fmu->m_client->fmi2SetupExperiment(toleranceDefined, tolerance,
						startTime, stopTimeDefined, stopTime));
	}

	return fmi2OK;
}

extern "C" fmi2Status fmi2EnterInitializationMode(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2EnterInitializationMode());
	}

	return fmi2Fatal;
}

extern "C" fmi2Status fmi2ExitInitializationMode(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2ExitInitializationMode());
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2Terminate(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2Terminate());
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2Reset(fmi2Component c)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2Reset());
	}
	return fmi2Fatal;
}

extern "C" void fmi2FreeInstance(fmi2Component c)
{
	notimplemented(c, "fmi2FreeInstance");
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

extern "C" fmi2Status fmi2SetDebugLogging(fmi2Component c,
		fmi2Boolean loggingOn, size_t nCategories,
		const fmi2String categories[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(
				fmu->m_client->fmi2SetDebugLogging(loggingOn, nCategories,
						categories));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetReal(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2GetReal(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetInteger(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2GetInteger(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetBoolean(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2GetBoolean(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetString(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2GetString(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetReal(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2SetReal(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetInteger(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2SetInteger(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetBoolean(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2SetBoolean(vr, nvr, value));
	}
	return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetString(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2SetString(vr, nvr, value));
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
extern "C" fmi2Status fmi2SerializedFMUstateSize(fmi2Component c,
		fmi2FMUstate FMUstate, size_t *size)
{
	notimplemented(c, "fmi2SerializedFMUstateSize");
	return fmi2Error;
}
extern "C" fmi2Status fmi2SerializeFMUstate(fmi2Component c,
		fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size)
{
	notimplemented(c, "fmi2SerializeFMUstate");
	return fmi2Error;
}
extern "C" fmi2Status fmi2DeSerializeFMUstate(fmi2Component c,
		const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate)
{
	notimplemented(c, "fmi2DeSerializeFMUstate");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
		const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
		const fmi2ValueReference vKnown_ref[], size_t nKnown,
		const fmi2Real dvKnown[], fmi2Real dvUnknown[])
{
	notimplemented(c, "fmi2GetDirectionalDerivative");
	return fmi2Error;
}

// ---------------------------------------------------------------------------
// Functions for FMI for Co-Simulation
// ---------------------------------------------------------------------------
#ifdef FMI_COSIMULATION
/* Simulating the slave */
extern "C" fmi2Status fmi2SetRealInputDerivatives(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[],
		const fmi2Real value[])
{
	notimplemented(c, "fmi2SetRealInputDerivatives");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c,
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[],
		fmi2Real value[])
{
	notimplemented(c, "fmi2GetRealOutputDerivatives");
	return fmi2Error;
}

extern "C" fmi2Status fmi2CancelStep(fmi2Component c)
{
	notimplemented(c, "fmi2CancelStep");
	return fmi2Error;
}

extern "C" fmi2Status fmi2DoStep(fmi2Component c,
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(
				fmu->m_client->fmi2DoStep(currentCommunicationPoint,
						communicationStepSize,
						noSetFMUStatePriorToCurrentPoint));
	}

	return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s,
		fmi2Status *value)
{
	notimplemented(c, "fmi2GetStatus");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s,
		fmi2Real *value)
{
	notimplemented(c, "fmi2GetRealStatus");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetIntegerStatus(fmi2Component c,
		const fmi2StatusKind s, fmi2Integer *value)
{
	notimplemented(c, "fmi2GetIntegerStatus");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetBooleanStatus(fmi2Component c,
		const fmi2StatusKind s, fmi2Boolean *value)
{
	notimplemented(c, "fmi2GetBooleanStatus");
	return fmi2Error;
}

extern "C" fmi2Status fmi2GetStringStatus(fmi2Component c,
		const fmi2StatusKind s, fmi2String *value)
{
	notimplemented(c, "fmi2GetStringStatus");
	return fmi2Error;
}

/* INTO cps specific*/
extern "C" fmi2Status fmi2GetMaxStepsize(fmi2Component c, fmi2Real* size)
{
	FmuContainer* fmu = getFmuContainer(c);

	if (fmu != NULL)
	{
		return convertStatus(fmu->m_client->fmi2GetMaxStepsize(size));
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
