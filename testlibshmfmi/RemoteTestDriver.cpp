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
#include "FmuProxy.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"
#include "IpcClient.h"

#define TD_SIZE 100

double reals[TD_SIZE] = {0};

bool bools[TD_SIZE] = {0};

int ints[TD_SIZE] = {0};

std::string strings[TD_SIZE];

void okReply(SharedFmiMessage* msg) {
  Fmi2StatusReply* r = new Fmi2StatusReply();
  r->set_status(Fmi2StatusReply_Status_Ok);
  msg->protoBufMsgSize = r->ByteSizeLong();
  r->SerializeWithCachedSizesToArray(msg->protoBufMsg);
}

FmiIpc::IpcClient* globalClient = NULL;

void remoteTestDriverSingle(const char* shmKey) {
  globalClient = NULL;
  remoteTestDriver(shmKey);
}

void remoteTestDriver(const char* shmKey) {
  bool success;

  FmiIpc::IpcClient* client = globalClient;

  // if (globalClient == NULL)
  {
    client = new FmiIpc::IpcClient(0, shmKey);
    client->connect(&success);
    if (!success) {
      remoteTestDriver(shmKey);
      return;
    }
  }

  // printf("Client successfully created\n");

  SharedFmiMessage* msg = client->getMessage(0);
  // printf("Client got message\n");

  switch (msg->cmd) {
    case sharedfmimemory::fmi2FreeInstance: {
      Fmi2Empty* r = new Fmi2Empty();
      msg->protoBufMsgSize = r->ByteSizeLong();
      r->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;

    case sharedfmimemory::fmi2SetDebugLogging:
    case sharedfmimemory::fmi2Instantiate:
    //	case sharedfmimemory::fmi2FreeInstance:
    case sharedfmimemory::fmi2SetupExperiment:
    case sharedfmimemory::fmi2EnterInitializationMode:
    case sharedfmimemory::fmi2ExitInitializationMode:
    case sharedfmimemory::fmi2Terminate:
    case sharedfmimemory::fmi2Reset:
    case sharedfmimemory::fmi2DoStep:
    // case sharedfmimemory::fmi2SetReal:
    // case sharedfmimemory::fmi2SetInteger:
    // case sharedfmimemory::fmi2SetBoolean:
    // case sharedfmimemory::fmi2SetString:
    case sharedfmimemory::fmi2GetStatus: {
      okReply(msg);

    } break;
    case sharedfmimemory::fmi2SetReal: {
      Fmi2SetRealRequest* r = new Fmi2SetRealRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        reals[id] = r->values(i);
      }

      okReply(msg);
    } break;
    case sharedfmimemory::fmi2SetInteger: {
      Fmi2SetIntegerRequest* r = new Fmi2SetIntegerRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        ints[id] = r->values(i);
      }
      okReply(msg);
    } break;
    case sharedfmimemory::fmi2SetBoolean: {
      Fmi2SetBooleanRequest* r = new Fmi2SetBooleanRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        bools[id] = r->values(i);
      }
      okReply(msg);
    } break;
    case sharedfmimemory::fmi2SetString: {
      Fmi2SetStringRequest* r = new Fmi2SetStringRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        strings[id] = new char[40];
        const char* s = r->values(i).c_str();
        strings[id] = *new std::string(s);
      }
      okReply(msg);
    } break;
    case sharedfmimemory::fmi2GetReal: {
      Fmi2GetRequest* r = new Fmi2GetRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2GetRealReply* reply = new Fmi2GetRealReply();

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        reply->add_values(reals[id]);
      }

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetBoolean: {
      Fmi2GetRequest* r = new Fmi2GetRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2GetBooleanReply* reply = new Fmi2GetBooleanReply();

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        reply->add_values(bools[id]);
      }

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetInteger: {
      Fmi2GetRequest* r = new Fmi2GetRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2GetIntegerReply* reply = new Fmi2GetIntegerReply();

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        reply->add_values(ints[id]);
      }

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetString: {
      Fmi2GetRequest* r = new Fmi2GetRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2GetStringReply* reply = new Fmi2GetStringReply();
      reply->set_valid(true);

      for (int i = 0; i < r->valuereference_size(); i++) {
        int id = r->valuereference(i);
        // reply->add_values(ints[id]);
        reply->add_values(strings[id]);
      }

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;

    case sharedfmimemory::fmi2GetMaxStepSize: {
      Fmi2GetMaxStepSizeReply* reply = new Fmi2GetMaxStepSizeReply();

      reply->set_maxstepsize(100);

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;

    case sharedfmimemory::fmi2GetRealStatus: {
      Fmi2StatusRequest* r = new Fmi2StatusRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2RealStatusReply* reply = new Fmi2RealStatusReply();

      if (r->status() == Fmi2StatusRequest::fmi2LastSuccessfulTime)
        reply->set_value(100.5);

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetIntegerStatus: {
      Fmi2StatusRequest* r = new Fmi2StatusRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2IntegerStatusReply* reply = new Fmi2IntegerStatusReply();

      if (r->status() == Fmi2StatusRequest::fmi2LastSuccessfulTime)
        reply->set_value(100);

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetBooleanStatus: {
      Fmi2StatusRequest* r = new Fmi2StatusRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2BooleanStatusReply* reply = new Fmi2BooleanStatusReply();

      if (r->status() == Fmi2StatusRequest::fmi2Terminated)
        reply->set_value(true);

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    case sharedfmimemory::fmi2GetStringStatus: {
      Fmi2StatusRequest* r = new Fmi2StatusRequest();
      r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

      Fmi2StringStatusReply* reply = new Fmi2StringStatusReply();

      if (r->status() == Fmi2StatusRequest::fmi2DoStepStatus)
        reply->set_value("waiting");

      msg->protoBufMsgSize = reply->ByteSizeLong();
      reply->SerializeWithCachedSizesToArray(msg->protoBufMsg);
    } break;
    default:

      break;
  }
  client->sendReply(msg);

  return;
}
