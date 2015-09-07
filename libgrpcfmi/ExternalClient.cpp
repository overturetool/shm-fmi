/*
 * ExternalClient.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: parallels
 */

#include "ExternalClient.h"

ExternalClient::ExternalClient(std::shared_ptr<ChannelInterface> channel) :
		stub_(Fmu::NewStub(channel))
{
}

ExternalClient::ExternalClient(std::string url) :
		stub_(
				Fmu::NewStub(
						grpc::CreateChannel(url, grpc::InsecureCredentials(),
								ChannelArguments())))
{
}

ExternalClient::~ExternalClient()
{
	// TODO Auto-generated destructor stub

}

ExternalClient::fmi2Status ExternalClient::getStatus(Fmi2StatusReply reply)
{
	switch (reply.status())
	{
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Discard:
		return ExternalClient::fmi2Status::fmi2Discard;
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Error:
		return ExternalClient::fmi2Status::fmi2Error;
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Fatal:
		return ExternalClient::fmi2Status::fmi2Fatal;
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Ok:
		return ExternalClient::fmi2Status::fmi2OK;
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Pending:
		return ExternalClient::fmi2Status::fmi2Pending;
	case service::Fmi2StatusReply_Status::Fmi2StatusReply_Status_Warning:
		return ExternalClient::fmi2Status::fmi2Warning;
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}


bool ExternalClient::fmi2Instantiate(fmi2String instanceName,  fmi2String fmuGUID,
	                            fmi2String fmuResourceLocation, const char* callbackAddress, int callbackPort,
	                            fmi2Boolean visible, fmi2Boolean loggingOn){

	InstantiateRequest request;
	request.set_instancename(instanceName);
	request.set_fmuguid(fmuGUID);
	request.set_fmuresourcelocation(fmuResourceLocation);
	request.set_callbackaddress(callbackAddress);
	request.set_callbackport(callbackPort);
	request.set_visible(visible);
	request.set_logginon(loggingOn);

	InstantiateReply reply;
	ClientContext context;

	Status status = stub_->fmi2Instantiate(&context, request, &reply);
	if (status.ok())
	{
		return true;
	}

	return false;

}

ExternalClient::fmi2Status ExternalClient::fmi2SetupExperiment(
		fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime,
		fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	SetupExperimentRequest request;
	request.set_tolerancedefined(toleranceDefined);
	request.set_tolerance(tolerance);
	request.set_starttime(startTime);
	request.set_stoptimedefined(stopTimeDefined);
	request.set_stoptime(stopTime);

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetupExperiment(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2EnterInitializationMode()
{
	Empty request;
	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2EnterInitializationMode(&context, request,
			&reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2ExitInitializationMode()
{
	Empty request;
	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2ExitInitializationMode(&context, request,
			&reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2Terminate()
{
	Empty request;
	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2Terminate(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}


ExternalClient::fmi2Status ExternalClient::fmi2Reset()
{
	Empty request;
	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2Reset(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}


ExternalClient::fmi2Status ExternalClient::fmi2SetDebugLogging(
		fmi2Boolean loggingOn, size_t nCategories,
		const fmi2String categories[])
{
	SetDebugLoggingRequest request;

	request.set_loggingon(loggingOn);
	request.set_categoriessize(nCategories);

	for (int i = 0; i < nCategories; i++)
	{
		request.add_catogories( categories[i]);
	}

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetDebugLogging(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

GetRequest createGetRequest(const ExternalClient::fmi2ValueReference vr[],
		size_t nvr)
{
	GetRequest request;

	request.set_size(nvr);

	for (int i = 0; i < nvr; i++)
	{
		request.add_valuereference( vr[i]);
	}
	return request;
}
//
// get
//
ExternalClient::fmi2Status ExternalClient::fmi2GetReal(
		const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	GetRequest request = createGetRequest(vr, nvr);

	GetRealReply reply;
	ClientContext context;

	Status status = stub_->fmi2GetReal(&context, request, &reply);
	if (status.ok())
	{
		for (int i = 0; i < reply.values_size(); ++i)
		{
			value[i] = reply.values(i);
		}
		return ExternalClient::fmi2Status::fmi2OK;
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2GetInteger(
		const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	GetRequest request = createGetRequest(vr, nvr);

	GetIntegerReply reply;
	ClientContext context;

	Status status = stub_->fmi2GetInteger(&context, request, &reply);
	if (status.ok())
	{
		for (int i = 0; i < reply.values_size(); ++i)
		{
			value[i] = reply.values(i);
		}
		return ExternalClient::fmi2Status::fmi2OK;
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2GetBoolean(
		const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	GetRequest request = createGetRequest(vr, nvr);

	GetBooleanReply reply;
	ClientContext context;

	Status status = stub_->fmi2GetBoolean(&context, request, &reply);
	if (status.ok())
	{
		for (int i = 0; i < reply.values_size(); ++i)
		{
			value[i] = reply.values(i);
		}
		return ExternalClient::fmi2Status::fmi2OK;
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2GetString(
		const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
	GetRequest request = createGetRequest(vr, nvr);

	GetStringReply reply;
	ClientContext context;

	Status status = stub_->fmi2GetString(&context, request, &reply);
	if (status.ok())
	{
		for (int i = 0; i < reply.values_size(); ++i)
		{
			value[i] = reply.values(i).c_str();
		}
		return ExternalClient::fmi2Status::fmi2OK;
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

//
// Set
//

ExternalClient::fmi2Status ExternalClient::fmi2SetReal(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	SetRealRequest request;

	for (int i = 0; i < nvr; i++)
	{
		request.add_valuereference( vr[i]);
	}

	for (int i = 0; i < nvr; i++)
	{
		request.add_values( value[i]);
	}

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetReal(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2SetInteger(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
	SetIntegerRequest request;

	for (int i = 0; i < nvr; i++)
	{
		request.add_valuereference( vr[i]);
	}

	for (int i = 0; i < nvr; i++)
	{
		request.add_values( value[i]);
	}

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetInteger(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2SetBoolean(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
	SetBooleanRequest request;

	for (int i = 0; i < nvr; i++)
	{
		request.add_valuereference(vr[i]);
	}

	for (int i = 0; i < nvr; i++)
	{
		request.add_values( value[i]);
	}

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetBoolean(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

ExternalClient::fmi2Status ExternalClient::fmi2SetString(
		const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
	SetStringRequest request;

	for (int i = 0; i < nvr; i++)
	{
		request.add_valuereference( vr[i]);
	}

	for (int i = 0; i < nvr; i++)
	{
		request.add_values( value[i]);
	}

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2SetString(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

//
// do step
//

ExternalClient::fmi2Status ExternalClient::fmi2DoStep(
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
	DoStepRequest request;
	request.set_currentcommunicationpoint(currentCommunicationPoint);
	request.set_communicationstepsize(communicationStepSize);

	Fmi2StatusReply reply;
	ClientContext context;

	Status status = stub_->fmi2DoStep(&context, request, &reply);
	if (status.ok())
	{
		return getStatus(reply);
	}

	return ExternalClient::fmi2Status::fmi2Fatal;
}

//
// INTOS specific
//

ExternalClient::fmi2Status ExternalClient::fmi2GetMaxStepsize(fmi2Real* size)
{
	Empty request;
		GetMaxStepSizeReply reply;
		ClientContext context;

		Status status = stub_->fmi2GetMaxStepSize(&context, request,
				&reply);
		if (status.ok())
		{
			*size = reply.maxstepsize();
			return ExternalClient::fmi2Status::fmi2OK;
			//return getStatus(reply);
		}

		return ExternalClient::fmi2Status::fmi2Fatal;
}
