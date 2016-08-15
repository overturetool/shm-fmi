/*
 * FmuProxy.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: parallels
 */

#include "FmuProxy.h"

FmuProxy::FmuProxy(std::string url)
{
	this->m_url = new std::string(url);
	this->server = new FmiIpc::Server();
}

FmuProxy::~FmuProxy()
{
	delete this->m_url;
	delete this->server;
}


bool FmuProxy::initialize()
{
	return this->server->create(this->m_url->c_str());
}

google::protobuf::Message* FmuProxy::send(
		sharedfmimemory::fmi2Command type, google::protobuf::Message* msg)
{

	sharedfmimemory::SharedFmiMessage m = sharedfmimemory::SharedFmiMessage();
	m.cmd = type;

	switch (type)
	{

	case sharedfmimemory::fmi2EnterInitializationMode:
	case sharedfmimemory::fmi2ExitInitializationMode:
	case sharedfmimemory::fmi2Terminate:
	case sharedfmimemory::fmi2Reset:
	case sharedfmimemory::fmi2GetMaxStepSize:
	{
		Fmi2Empty* im = (Fmi2Empty*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2SetDebugLogging:
	{
		Fmi2SetDebugLoggingRequest* im = (Fmi2SetDebugLoggingRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
	case sharedfmimemory::fmi2Instantiate:
	{
		Fmi2InstantiateRequest* im = (Fmi2InstantiateRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
		//case sharedfmimemory::fmi2FreeInstance,
	case sharedfmimemory::fmi2SetupExperiment:
	{
		Fmi2SetupExperimentRequest* im = (Fmi2SetupExperimentRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}

		break;

	case sharedfmimemory::fmi2DoStep:
	{
		Fmi2DoStepRequest* im = (Fmi2DoStepRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2GetReal:
	case sharedfmimemory::fmi2GetInteger:
	case sharedfmimemory::fmi2GetBoolean:
	case sharedfmimemory::fmi2GetString:
	{
		Fmi2GetRequest* im = (Fmi2GetRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2SetReal:
	{
		Fmi2SetRealRequest* im = (Fmi2SetRealRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
	case sharedfmimemory::fmi2SetInteger:
	{
		Fmi2SetIntegerRequest* im = (Fmi2SetIntegerRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
	case sharedfmimemory::fmi2SetBoolean:
	{
		Fmi2SetBooleanRequest* im = (Fmi2SetBooleanRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2SetString:
	{
		Fmi2SetStringRequest* im = (Fmi2SetStringRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
	case sharedfmimemory::fmi2GetStatus:
	case sharedfmimemory::fmi2GetRealStatus:
	case sharedfmimemory::fmi2GetIntegerStatus:
	case sharedfmimemory::fmi2GetBooleanStatus:
	case sharedfmimemory::fmi2GetStringStatus:
	{
		Fmi2StatusRequest* im = (Fmi2StatusRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	}

	//printf("Sending type: %i Size: %i\n",m.cmd,m.protoBufMsgSize);
	SharedFmiMessage* reply = this->server->send(&m, INFINITE);

	if (reply == NULL)
		return NULL; //timeout

	switch (type)
	{

	case sharedfmimemory::fmi2SetDebugLogging:
	case sharedfmimemory::fmi2Instantiate:
//	case sharedfmimemory::fmi2FreeInstance:
	case sharedfmimemory::fmi2SetupExperiment:
	case sharedfmimemory::fmi2EnterInitializationMode:
	case sharedfmimemory::fmi2ExitInitializationMode:
	case sharedfmimemory::fmi2Terminate:
	case sharedfmimemory::fmi2Reset:
	case sharedfmimemory::fmi2DoStep:
	case sharedfmimemory::fmi2SetReal:
	case sharedfmimemory::fmi2SetInteger:
	case sharedfmimemory::fmi2SetBoolean:
	case sharedfmimemory::fmi2SetString:
	case sharedfmimemory::fmi2GetStatus:
	{
		Fmi2StatusReply* r = new Fmi2StatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	case sharedfmimemory::fmi2GetReal:
	{
		Fmi2GetRealReply* r = new Fmi2GetRealReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetInteger:
	{
		Fmi2GetIntegerReply* r = new Fmi2GetIntegerReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetBoolean:
	{
		Fmi2GetBooleanReply* r = new Fmi2GetBooleanReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetString:
	{
		Fmi2GetStringReply* r = new Fmi2GetStringReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	case sharedfmimemory::fmi2GetMaxStepSize:
	{
		Fmi2GetMaxStepSizeReply* r = new Fmi2GetMaxStepSizeReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

		//case sharedfmimemory::fmi2GetStatus:
	case sharedfmimemory::fmi2GetRealStatus:
	{
		Fmi2RealStatusReply* r = new Fmi2RealStatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetIntegerStatus:
	{
		Fmi2IntegerStatusReply* r = new Fmi2IntegerStatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetBooleanStatus:
	{
		Fmi2BooleanStatusReply* r = new Fmi2BooleanStatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetStringStatus:
	{
		Fmi2StringStatusReply* r = new Fmi2StringStatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	}

	return NULL;
}

FmuProxy::fmi2Status FmuProxy::sendRetStatus(
		sharedfmimemory::fmi2Command type, google::protobuf::Message* msg)
{

	Fmi2StatusReply* reply = (Fmi2StatusReply*) send(type, msg);

	if (reply == NULL)
		return fmi2Fatal;

	return getStatus(reply);
}

FmuProxy::fmi2Status FmuProxy::getStatus(Fmi2StatusReply* reply)
{
	switch (reply->status())
	{
	case Fmi2StatusReply_Status_Discard:
		return fmi2Discard;
	case Fmi2StatusReply_Status_Error:
		return fmi2Error;
	case Fmi2StatusReply_Status_Fatal:
		return fmi2Fatal;
	case Fmi2StatusReply_Status_Ok:
		return fmi2OK;
	case Fmi2StatusReply_Status_Pending:
		return fmi2Pending;
	case Fmi2StatusReply_Status_Warning:
		return fmi2Warning;
	case Fmi2StatusReply_Status_Fmi2StatusReply_Status_INT_MIN_SENTINEL_DO_NOT_USE_:
	case Fmi2StatusReply_Status_Fmi2StatusReply_Status_INT_MAX_SENTINEL_DO_NOT_USE_:
		//ignore
		break;
	}

	return fmi2Fatal;
}

bool FmuProxy::fmi2Instantiate(fmi2String instanceName,
		fmi2String fmuGUID, fmi2String fmuResourceLocation,
		const char* callbackAddress, fmi2Boolean visible,
		fmi2Boolean loggingOn)
{

	Fmi2InstantiateRequest request;
	request.set_instancename(instanceName);
	request.set_fmuguid(fmuGUID);
	request.set_fmuresourcelocation(fmuResourceLocation);
	request.set_callbackshmname(callbackAddress);
	request.set_visible(visible);
	request.set_logginon(loggingOn);

	Fmi2StatusReply* reply = (Fmi2StatusReply*) send(
			sharedfmimemory::fmi2Instantiate, &request);

	if (reply == NULL || getStatus(reply) != fmi2OK)
		return false;

	return true;

}

FmuProxy::fmi2Status FmuProxy::fmi2SetupExperiment(
		fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime,
		fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	Fmi2SetupExperimentRequest request;
	request.set_tolerancedefined(toleranceDefined);
	request.set_tolerance(tolerance);
	request.set_starttime(startTime);
	request.set_stoptimedefined(stopTimeDefined);
	request.set_stoptime(stopTime);

	return sendRetStatus(sharedfmimemory::fmi2SetupExperiment, &request);

}

FmuProxy::fmi2Status FmuProxy::fmi2EnterInitializationMode()
{
	Fmi2Empty request;
	return sendRetStatus(sharedfmimemory::fmi2EnterInitializationMode, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2ExitInitializationMode()
{
	Fmi2Empty request;
	return sendRetStatus(sharedfmimemory::fmi2ExitInitializationMode, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2Terminate()
{
	Fmi2Empty request;
	return sendRetStatus(sharedfmimemory::fmi2Terminate, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2Reset()
{
	Fmi2Empty request;
	return sendRetStatus(sharedfmimemory::fmi2Reset, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2SetDebugLogging(
		fmi2Boolean loggingOn, size_t nCategories,
		const fmi2String categories[])
{
	Fmi2SetDebugLoggingRequest request;

	request.set_loggingon(loggingOn);
	request.set_categoriessize(nCategories);

	for (size_t i = 0; i < nCategories; i++)
	{
		request.add_catogories(categories[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetDebugLogging, &request);
}

Fmi2GetRequest createGetRequest(const FmuProxy::fmi2ValueReference vr[],
		size_t nvr)
{
	Fmi2GetRequest request;

	request.set_size(nvr);

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_valuereference(vr[i]);
	}
	return request;
}

//
// get
//
FmuProxy::fmi2Status FmuProxy::fmi2GetReal(
		const fmi2ValueReference vr[], size_t nvr, fmi2Real value[])
{
	Fmi2GetRequest request = createGetRequest(vr, nvr);

	Fmi2GetRealReply* reply = (Fmi2GetRealReply*) send(
			sharedfmimemory::fmi2GetReal, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i)
	{
		value[i] = reply->values(i);
	}
	return fmi2OK;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetInteger(
		const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[])
{
	Fmi2GetRequest request = createGetRequest(vr, nvr);

	Fmi2GetIntegerReply* reply = (Fmi2GetIntegerReply*) send(
			sharedfmimemory::fmi2GetInteger, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i)
	{
		value[i] = reply->values(i);
	}
	return fmi2OK;

}

FmuProxy::fmi2Status FmuProxy::fmi2GetBoolean(
		const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[])
{
	Fmi2GetRequest request = createGetRequest(vr, nvr);

	Fmi2GetBooleanReply* reply = (Fmi2GetBooleanReply*) send(
			sharedfmimemory::fmi2GetBoolean, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i)
	{
		value[i] = reply->values(i);
	}
	return fmi2OK;

}

FmuProxy::fmi2Status FmuProxy::fmi2GetString(
		const fmi2ValueReference vr[], size_t nvr, fmi2String value[])
{
	Fmi2GetRequest request = createGetRequest(vr, nvr);

	Fmi2GetStringReply* reply = (Fmi2GetStringReply*) send(
			sharedfmimemory::fmi2GetString, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i)
	{
		value[i] = reply->values(i).c_str();
	}
	return fmi2OK;

}

//
// Set
//

FmuProxy::fmi2Status FmuProxy::fmi2SetReal(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[])
{
	Fmi2SetRealRequest* request = new Fmi2SetRealRequest();

	for (size_t i = 0; i < nvr; i++)
	{
		request->add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++)
	{
		request->add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetReal, request);
}

FmuProxy::fmi2Status FmuProxy::fmi2SetInteger(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[])
{
	Fmi2SetIntegerRequest request;

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetInteger, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2SetBoolean(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[])
{
	Fmi2SetBooleanRequest request;

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetBoolean, &request);
}

FmuProxy::fmi2Status FmuProxy::fmi2SetString(
		const fmi2ValueReference vr[], size_t nvr, const fmi2String value[])
{
	Fmi2SetStringRequest request;

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++)
	{
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetString, &request);
}

//
// do step
//

FmuProxy::fmi2Status FmuProxy::fmi2DoStep(
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
	Fmi2DoStepRequest request;
	request.set_currentcommunicationpoint(currentCommunicationPoint);
	request.set_communicationstepsize(communicationStepSize);

	return sendRetStatus(sharedfmimemory::fmi2DoStep, &request);
}

//
// INTOS specific
//

FmuProxy::fmi2Status FmuProxy::fmi2GetMaxStepsize(fmi2Real* size)
{
	Fmi2Empty request;
	Fmi2GetMaxStepSizeReply* reply = (Fmi2GetMaxStepSizeReply*) send(
			sharedfmimemory::fmi2GetMaxStepSize, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*size = reply->maxstepsize();
	return fmi2OK;
}

//
// get status
//

Fmi2StatusRequest FmuProxy::createGetStatusRequest(const fmi2StatusKind s)
{
	Fmi2StatusRequest request;

	switch (s)
	{
	case fmi2StatusKind::fmi2DoStepStatus:
		request.set_status(Fmi2StatusRequest::fmi2DoStepStatus);
		break;
	case fmi2StatusKind::fmi2LastSuccessfulTime:
		request.set_status(Fmi2StatusRequest::fmi2LastSuccessfulTime);
		break;
	case fmi2StatusKind::fmi2PendingStatus:
		request.set_status(Fmi2StatusRequest::fmi2PendingStatus);
		break;
	case fmi2StatusKind::fmi2Terminated:
		request.set_status(Fmi2StatusRequest::fmi2Terminated);
		break;
	}
	return request;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetStatus(const fmi2StatusKind s,
		fmi2Status* value)
{
	Fmi2StatusRequest request = createGetStatusRequest(s);
	*value = sendRetStatus(sharedfmimemory::fmi2GetStatus, &request);
	return *value;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetRealStatus(
		const fmi2StatusKind s, fmi2Real* value)
{
	Fmi2StatusRequest request = createGetStatusRequest(s);

	Fmi2RealStatusReply* reply = (Fmi2RealStatusReply*) send(
			sharedfmimemory::fmi2GetRealStatus, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*value = reply->value();
	return fmi2OK;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetIntegerStatus(
		const fmi2StatusKind s, fmi2Integer* value)
{
	Fmi2StatusRequest request = createGetStatusRequest(s);

	Fmi2IntegerStatusReply* reply = (Fmi2IntegerStatusReply*) send(
			sharedfmimemory::fmi2GetIntegerStatus, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*value = reply->value();
	return fmi2OK;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetBooleanStatus(
		const fmi2StatusKind s, fmi2Boolean* value)
{
	Fmi2StatusRequest request = createGetStatusRequest(s);

	Fmi2BooleanStatusReply* reply = (Fmi2BooleanStatusReply*) send(
			sharedfmimemory::fmi2GetBooleanStatus, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*value = reply->value();
	return fmi2OK;
}

FmuProxy::fmi2Status FmuProxy::fmi2GetStringStatus(
		const fmi2StatusKind s, fmi2String* value)
{
	Fmi2StatusRequest request = createGetStatusRequest(s);

	Fmi2StringStatusReply* reply = (Fmi2StringStatusReply*) send(
			sharedfmimemory::fmi2GetStringStatus, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*value = reply->value().c_str();
	return fmi2OK;
}
