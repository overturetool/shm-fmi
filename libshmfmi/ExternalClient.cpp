/*
 * ExternalClient.cpp
 *
 *  Created on: Aug 20, 2015
 *      Author: parallels
 */

#include "ExternalClient.h"

/*ExternalClient::ExternalClient(std::shared_ptr<ChannelInterface> channel) :
 stub_(Fmu::NewStub(channel))
 {
 }*/

ExternalClient::ExternalClient(std::string url) {
	this->server = new FmiIpc::Server(); //FIXME &url.c_str());
}

ExternalClient::~ExternalClient() {
	// TODO Auto-generated destructor stub

}

google::protobuf::Message* ExternalClient::send(
		sharedfmimemory::fmi2Command type, google::protobuf::Message* msg) {

	sharedfmimemory::SharedFmiMessage m = sharedfmimemory::SharedFmiMessage();
	m.cmd = type;

	switch (type) {

	case sharedfmimemory::fmi2EnterInitializationMode:
	case sharedfmimemory::fmi2ExitInitializationMode:
	case sharedfmimemory::fmi2Terminate:
	case sharedfmimemory::fmi2Reset:
	case sharedfmimemory::fmi2GetMaxStepSize: {
		Empty* im = (Empty*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);


	}
		break;

	case sharedfmimemory::fmi2SetDebugLogging: {
		SetDebugLoggingRequest* im = (SetDebugLoggingRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);
printf("buf size set deubg logging %i\n",m.protoBufMsgSize);
	}
		break;
	case sharedfmimemory::fmi2Instantiate: {
		InstantiateRequest* im = (InstantiateRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
		//case sharedfmimemory::fmi2FreeInstance,
	case sharedfmimemory::fmi2SetupExperiment: {
		SetupExperimentRequest* im = (SetupExperimentRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}

		break;

	case sharedfmimemory::fmi2DoStep: {
		DoStepRequest* im = (DoStepRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2GetReal:
	case sharedfmimemory::fmi2GetInteger:
	case sharedfmimemory::fmi2GetBoolean:
	case sharedfmimemory::fmi2GetString: {
		GetRequest* im = (GetRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2SetReal: {
		SetRealRequest* im = (SetRealRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);


	}
		break;
	case sharedfmimemory::fmi2SetInteger: {
		SetIntegerRequest* im = (SetIntegerRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;
	case sharedfmimemory::fmi2SetBoolean: {
		SetBooleanRequest* im = (SetBooleanRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	case sharedfmimemory::fmi2SetString: {
		SetStringRequest* im = (SetStringRequest*) msg;
		m.protoBufMsgSize = im->ByteSize();
		im->SerializeWithCachedSizesToArray(m.protoBufMsg);

	}
		break;

	}

	//printf("Sending type: %i Size: %i\n",m.cmd,m.protoBufMsgSize);
	SharedFmiMessage* reply = this->server->send(&m,INFINITE);

	if(reply==NULL)
		return NULL;//timeout

	switch (type) {

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
	case sharedfmimemory::fmi2SetString: {
		Fmi2StatusReply* r = new Fmi2StatusReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	case sharedfmimemory::fmi2GetReal: {
		GetRealReply* r = new GetRealReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetInteger: {
		GetIntegerReply* r = new GetIntegerReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetBoolean: {
		GetBooleanReply* r = new GetBooleanReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}
	case sharedfmimemory::fmi2GetString: {
		GetStringReply* r = new GetStringReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	case sharedfmimemory::fmi2GetMaxStepSize: {
		GetMaxStepSizeReply* r = new GetMaxStepSizeReply();
		r->ParseFromArray(reply->protoBufMsg, reply->protoBufMsgSize);
		return r;
	}

	}

	return NULL;
}

ExternalClient::fmi2Status ExternalClient::sendRetStatus(
		sharedfmimemory::fmi2Command type, google::protobuf::Message* msg) {

	Fmi2StatusReply* reply = (Fmi2StatusReply*) send(type, msg);

	if (reply == NULL)
		return fmi2Fatal;

	return getStatus(reply);
}

ExternalClient::fmi2Status ExternalClient::getStatus(Fmi2StatusReply* reply) {
	switch (reply->status()) {
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

bool ExternalClient::fmi2Instantiate(fmi2String instanceName,
		fmi2String fmuGUID, fmi2String fmuResourceLocation,
		const char* callbackAddress, int callbackPort, fmi2Boolean visible,
		fmi2Boolean loggingOn) {

	InstantiateRequest request;
	request.set_instancename(instanceName);
	request.set_fmuguid(fmuGUID);
	request.set_fmuresourcelocation(fmuResourceLocation);
	request.set_callbackaddress(callbackAddress);
	request.set_callbackport(callbackPort);
	request.set_visible(visible);
	request.set_logginon(loggingOn);

	Fmi2StatusReply* reply = (Fmi2StatusReply*) send(
			sharedfmimemory::fmi2Instantiate, &request);

	if (reply == NULL || getStatus(reply) != fmi2OK)
		return false;

	return true;

}

ExternalClient::fmi2Status ExternalClient::fmi2SetupExperiment(
		fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime,
		fmi2Boolean stopTimeDefined, fmi2Real stopTime) {
	SetupExperimentRequest request;
	request.set_tolerancedefined(toleranceDefined);
	request.set_tolerance(tolerance);
	request.set_starttime(startTime);
	request.set_stoptimedefined(stopTimeDefined);
	request.set_stoptime(stopTime);

	return sendRetStatus(sharedfmimemory::fmi2SetupExperiment, &request);

}

ExternalClient::fmi2Status ExternalClient::fmi2EnterInitializationMode() {
	Empty request;
	return sendRetStatus(sharedfmimemory::fmi2EnterInitializationMode, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2ExitInitializationMode() {
	Empty request;
	return sendRetStatus(sharedfmimemory::fmi2ExitInitializationMode, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2Terminate() {
	Empty request;
	return sendRetStatus(sharedfmimemory::fmi2Terminate, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2Reset() {
	Empty request;
	return sendRetStatus(sharedfmimemory::fmi2Reset, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2SetDebugLogging(
		fmi2Boolean loggingOn, size_t nCategories,
		const fmi2String categories[]) {
	SetDebugLoggingRequest request;

	request.set_loggingon(loggingOn);
	request.set_categoriessize(nCategories);

	for (size_t i = 0; i < nCategories; i++) {
		request.add_catogories(categories[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetDebugLogging, &request);
}

GetRequest createGetRequest(const ExternalClient::fmi2ValueReference vr[],
		size_t nvr) {
	GetRequest request;

	request.set_size(nvr);

	for (size_t i = 0; i < nvr; i++) {
		request.add_valuereference(vr[i]);
	}
	return request;
}

//
// get
//
ExternalClient::fmi2Status ExternalClient::fmi2GetReal(
		const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]) {
	GetRequest request = createGetRequest(vr, nvr);

	GetRealReply* reply = (GetRealReply*) send(sharedfmimemory::fmi2GetReal,
			&request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i) {
		value[i] = reply->values(i);
	}
	return fmi2OK;
}

ExternalClient::fmi2Status ExternalClient::fmi2GetInteger(
		const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]) {
	GetRequest request = createGetRequest(vr, nvr);

	GetIntegerReply* reply = (GetIntegerReply*) send(
			sharedfmimemory::fmi2GetInteger, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i) {
		value[i] = reply->values(i);
	}
	return fmi2OK;

}

ExternalClient::fmi2Status ExternalClient::fmi2GetBoolean(
		const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]) {
	GetRequest request = createGetRequest(vr, nvr);

	GetBooleanReply* reply = (GetBooleanReply*) send(
			sharedfmimemory::fmi2GetBoolean, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i) {
		value[i] = reply->values(i);
	}
	return fmi2OK;

}

ExternalClient::fmi2Status ExternalClient::fmi2GetString(
		const fmi2ValueReference vr[], size_t nvr, fmi2String value[]) {
	GetRequest request = createGetRequest(vr, nvr);

	GetStringReply* reply = (GetStringReply*) send(
			sharedfmimemory::fmi2GetString, &request);

	if (reply == NULL)
		return fmi2Fatal;

	for (int i = 0; i < reply->values_size(); ++i) {
		value[i] = reply->values(i).c_str();
	}
	return fmi2OK;

}

//
// Set
//

ExternalClient::fmi2Status ExternalClient::fmi2SetReal(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]) {
	SetRealRequest* request = new SetRealRequest();

	for (size_t i = 0; i < nvr; i++) {
		request->add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++) {
		request->add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetReal, request);
}

ExternalClient::fmi2Status ExternalClient::fmi2SetInteger(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]) {
	SetIntegerRequest request;

	for (size_t i = 0; i < nvr; i++) {
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++) {
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetInteger, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2SetBoolean(
		const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]) {
	SetBooleanRequest request;

	for (size_t i = 0; i < nvr; i++) {
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++) {
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetBoolean, &request);
}

ExternalClient::fmi2Status ExternalClient::fmi2SetString(
		const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]) {
	SetStringRequest request;

	for (size_t i = 0; i < nvr; i++) {
		request.add_valuereference(vr[i]);
	}

	for (size_t i = 0; i < nvr; i++) {
		request.add_values(value[i]);
	}

	return sendRetStatus(sharedfmimemory::fmi2SetString, &request);
}

//
// do step
//

ExternalClient::fmi2Status ExternalClient::fmi2DoStep(
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint) {
	DoStepRequest request;
	request.set_currentcommunicationpoint(currentCommunicationPoint);
	request.set_communicationstepsize(communicationStepSize);

	return sendRetStatus(sharedfmimemory::fmi2DoStep, &request);
}

//
// INTOS specific
//

ExternalClient::fmi2Status ExternalClient::fmi2GetMaxStepsize(fmi2Real* size) {
	Empty request;
	GetMaxStepSizeReply* reply = (GetMaxStepSizeReply*) send(
			sharedfmimemory::fmi2GetMaxStepSize, &request);

	if (reply == NULL)
		return fmi2Fatal;

	*size = reply->maxstepsize();
	return fmi2OK;
}
