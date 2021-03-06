/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */

#ifndef SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_
#define SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_

#include <iostream>
#include <memory>
#include <string>

#include "service.pb.h"
#include "IpcServer.h"
#include "SharedFmiMessage.h"

using service::Fmi2GetRequest;
using service::Fmi2GetRealReply;
using service::Fmi2StatusReply;
using service::Fmi2Empty;
using service::Fmi2SetDebugLoggingRequest;
using service::Fmi2GetRequest;
using service::Fmi2GetRealReply;
using service::Fmi2StatusReply;
using namespace service;

class FmuProxy {
  typedef void* fmi2Component;
  typedef const char* fmi2String;
  typedef int fmi2Boolean;
  typedef double fmi2Real;

  typedef int fmi2Integer;

 public:
  typedef enum {
    fmi2OK,
    fmi2Warning,
    fmi2Discard,
    fmi2Error,
    fmi2Fatal,
    fmi2Pending
  } fmi2Status;
  typedef unsigned int fmi2ValueReference;

  typedef enum {
    fmi2DoStepStatus,
    fmi2PendingStatus,
    fmi2LastSuccessfulTime,
    fmi2Terminated
  } fmi2StatusKind;

  // FmuProxy(std::shared_ptr<ChannelInterface> channel);
  FmuProxy(int id, std::string url);
  virtual ~FmuProxy();

  bool initialize();

  bool fmi2Instantiate(fmi2String instanceName, fmi2String fmuGUID,
                       fmi2String fmuResourceLocation,
                       const char* callbackAddress, fmi2Boolean visible,
                       fmi2Boolean loggingOn);

  fmi2Status fmi2SetupExperiment(fmi2Boolean toleranceDefined,
                                 fmi2Real tolerance, fmi2Real startTime,
                                 fmi2Boolean stopTimeDefined,
                                 fmi2Real stopTime);

  fmi2Status fmi2EnterInitializationMode();
  fmi2Status fmi2ExitInitializationMode();
  fmi2Status fmi2Terminate();
  fmi2Status fmi2Reset();
  void fmi2FreeInstance();
  fmi2Status fmi2SetDebugLogging(fmi2Boolean loggingOn, size_t nCategories,
                                 const fmi2String categories[]);
  fmi2Status fmi2GetReal(const fmi2ValueReference vr[], size_t nvr,
                         fmi2Real value[]);
  fmi2Status fmi2GetInteger(const fmi2ValueReference vr[], size_t nvr,
                            fmi2Integer value[]);
  fmi2Status fmi2GetBoolean(const fmi2ValueReference vr[], size_t nvr,
                            fmi2Boolean value[]);
  fmi2Status fmi2GetString(const fmi2ValueReference vr[], size_t nvr,
                           fmi2String value[]);
  fmi2Status fmi2SetReal(const fmi2ValueReference vr[], size_t nvr,
                         const fmi2Real value[]);
  fmi2Status fmi2SetInteger(const fmi2ValueReference vr[], size_t nvr,
                            const fmi2Integer value[]);
  fmi2Status fmi2SetBoolean(const fmi2ValueReference vr[], size_t nvr,
                            const fmi2Boolean value[]);
  fmi2Status fmi2SetString(const fmi2ValueReference vr[], size_t nvr,
                           const fmi2String value[]);

  fmi2Status fmi2DoStep(fmi2Real currentCommunicationPoint,
                        fmi2Real communicationStepSize,
                        fmi2Boolean noSetFMUStatePriorToCurrentPoint);

  // status calls
  fmi2Status fmi2GetStatus(const fmi2StatusKind s, fmi2Status* value);
  fmi2Status fmi2GetRealStatus(const fmi2StatusKind s, fmi2Real* value);
  fmi2Status fmi2GetIntegerStatus(const fmi2StatusKind s, fmi2Integer* value);
  fmi2Status fmi2GetBooleanStatus(const fmi2StatusKind s, fmi2Boolean* value);
  fmi2Status fmi2GetStringStatus(const fmi2StatusKind s, fmi2String* value);

  // INTO specific
  fmi2Status fmi2GetMaxStepsize(fmi2Real* size);

  FmiIpc::IpcBase* getChannel();

 private:
  std::string* m_url;
  FmiIpc::IpcServer* server;
  FmuProxy::fmi2Status getStatus(Fmi2StatusReply*);

  google::protobuf::Message* send(sharedfmimemory::fmi2Command type,
                                  google::protobuf::Message* msg);
  FmuProxy::fmi2Status sendRetStatus(sharedfmimemory::fmi2Command type,
                                     google::protobuf::Message* msg);

  Fmi2StatusRequest createGetStatusRequest(const fmi2StatusKind s);
};

#endif /* SRC_MAIN_NATIVE_SRC_EXTERNALCLIENT_H_ */
