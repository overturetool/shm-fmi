/*
 * ExternalClient.h
 *
 *  Created on: Aug 20, 2015
 *      Author: parallels
 */

#ifndef SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_
#define SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_


#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpc++/channel_arguments.h>
#include <grpc++/channel_interface.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/credentials.h>
#include <grpc++/status.h>
#include "service.grpc.pb.h"

using grpc::ChannelArguments;
using grpc::ChannelInterface;
using grpc::ClientContext;
using grpc::Status;
using service::Fmu;
using service::GetRequest;
using service::GetRealReply;
using service::Fmi2StatusReply;
using service::Empty;
using service::SetDebugLoggingRequest;
using service::GetRequest;
using service::GetRealReply;
using namespace service;

class ExternalClient {
typedef 	void* fmi2Component;
typedef		const char* fmi2String;
typedef 	int fmi2Boolean;
typedef 	double fmi2Real;


typedef int             fmi2Integer;

public:
typedef enum {
    fmi2OK,
    fmi2Warning,
    fmi2Discard,
    fmi2Error,
    fmi2Fatal,
    fmi2Pending
} fmi2Status;
typedef unsigned int    fmi2ValueReference;
	ExternalClient(std::shared_ptr<ChannelInterface> channel);
	ExternalClient(std::string url);
	virtual ~ExternalClient();

	bool fmi2Instantiate(fmi2String instanceName,  fmi2String fmuGUID,
	                            fmi2String fmuResourceLocation, const char* callbackAddress, int callbackPort,
	                            fmi2Boolean visible, fmi2Boolean loggingOn);

	fmi2Status fmi2SetupExperiment( fmi2Boolean toleranceDefined, fmi2Real tolerance,
	                            fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime);

	fmi2Status fmi2EnterInitializationMode();
	fmi2Status fmi2ExitInitializationMode();
	fmi2Status fmi2Terminate();
	fmi2Status fmi2Reset();
	//void fmi2FreeInstance(fmi2Component c);
	fmi2Status fmi2SetDebugLogging( fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]);
	fmi2Status fmi2GetReal ( const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);
	fmi2Status fmi2GetInteger( const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]) ;
	fmi2Status fmi2GetBoolean( const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
	fmi2Status fmi2GetString ( const fmi2ValueReference vr[], size_t nvr, fmi2String value[]) ;
	fmi2Status fmi2SetReal ( const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);
	fmi2Status fmi2SetInteger( const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]) ;
	fmi2Status fmi2SetBoolean( const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
	fmi2Status fmi2SetString ( const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]);

	fmi2Status fmi2DoStep( fmi2Real currentCommunicationPoint,
	                    fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint);

	//INTO specific
	fmi2Status fmi2GetMaxStepsize(fmi2Real* size);

private:
	std::unique_ptr<Fmu::Stub> stub_;
	ExternalClient::fmi2Status getStatus(Fmi2StatusReply);
};

#endif /* SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_ */
