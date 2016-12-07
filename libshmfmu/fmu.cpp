/*
 * fmi.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: parallels
 */

#include <stdio.h>

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
#include "IpcClient.h"

// static std::map <int, FmuProxy> g_clients;
#include <vector>
#include "FmuContainer.h"
#include <sstream>
#include "JavaLauncher.h"
#include "ConfigFile.h"
#include "uri.h"

extern "C" {

#include "uuid4.h"
}

#ifdef _WIN32
#include <io.h>
#define access _access_s
#else
#include <unistd.h>
#endif

bool FileExists(const std::string& Filename) {
  return access(Filename.c_str(), 0) == 0;
}

static int currentId = 0;

std::vector<FmuContainer*> g_clients;

static FmuContainer* getFmuContainer(fmi2Component c) {
  for (auto& container : g_clients) {
    if (container == c) {
      return (FmuContainer*)c;
    }
  }
  return NULL;
}

static fmi2Status convertStatus(FmuProxy::fmi2Status status) {
  switch (status) {
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

static FmuProxy::fmi2StatusKind convertStatusKind(fmi2StatusKind kind) {
  switch (kind) {
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

static void notimplemented(fmi2Component c, fmi2String message) {
  FmuContainer* fmu = (FmuContainer*)c;

  if (fmu != NULL && fmu->m_functions != NULL &&
      fmu->m_functions->logger != NULL) {
    std::string base("Not implemented: ");
    std::string m(message);
    fmu->m_functions->logger(fmu->componentEnvironment, fmu->m_name, fmi2Error,
                             "error", (base + m).c_str());
  }
}

void sleepcp(int milliseconds)  // cross-platform sleep function
{
#ifdef WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds * 1000);
#endif  // win32
}

void callbackThreadFunc(FmuContainer* container, const char* shmCallbackKey) {
  if (container->m_functions == NULL ||
      container->m_functions->logger == NULL) {
    return;
  }

  FmiIpc::IpcClient* callbackClient = NULL;
  bool success = false;
  while (container->active && !success) {
    callbackClient = new FmiIpc::IpcClient(container->id, shmCallbackKey);

    callbackClient->connect(&success);
    if (!success) {
      delete callbackClient;
      callbackClient = NULL;
      printf("Unable to connect to callback shm: %s retry in 1 sec.\n\n",
             shmCallbackKey);
      sleepcp(1000);
    }
  }

  //	printf("callbackThreadFunc connected.");fflush(stdout);

  delete shmCallbackKey;

  if (callbackClient != NULL) {
    while (container->active) {
      SharedFmiMessage* msg = callbackClient->getMessage(10);

      if (msg == NULL) {
        //				printf("callbackThreadFunc got message
        // NULL.\n");fflush(stdout);
        continue;
      }
      //			printf("callbackThreadFunc got message %d, size
      //%d.",msg->cmd,msg->protoBufMsgSize);fflush(stdout);
      if (msg->cmd == sharedfmimemory::fmi2Log) {
        Fmi2LogReply* r = new Fmi2LogReply();
        r->ParseFromArray(msg->protoBufMsg, msg->protoBufMsgSize);

        fmi2Status status = fmi2Status::fmi2Error;

        switch (r->status()) {
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

        //				printf("Log message received;
        //%s\n",r->value().c_str());fflush(stdout);
        container->m_functions->logger(
            container->componentEnvironment, container->m_name, status,
            r->category().c_str(), r->value().c_str());

        delete r;
        SharedFmiMessage* msgReply = new SharedFmiMessage();

        Fmi2Empty request;

        msgReply->protoBufMsgSize = request.ByteSize();
        request.SerializeWithCachedSizesToArray(msgReply->protoBufMsg);
        msgReply->cmd = sharedfmimemory::fmi2Log;
        callbackClient->sendReply(msgReply);
        delete msgReply;
      }
    }
  }
  delete callbackClient;
}

#define LOG(functions, name, status, category, message, args...)       \
  if (functions != NULL) {                                             \
    if (functions->logger != NULL) {                                   \
      functions->logger(functions->componentEnvironment, name, status, \
                        category, message, args);                      \
    }                                                                  \
  } else {                                                             \
    fprintf(stderr, "Name '%s', Category: '%s'\n", name, category);    \
    fprintf(stderr, message, args);                                    \
    fprintf(stderr, "\n");                                             \
  }

int fmuInternalDebugPrint(int sender, const char* format, ...) {
  va_list args;
  va_start(args, format);
  for (int i = 0; i < g_clients.size(); i++) {
    FmuContainer* c = g_clients.at(i);
    if (c && c->m_proxy->getChannel()->getId() == sender) {
      g_clients.at(i)->m_functions->logger(c->componentEnvironment, c->m_name,
                                           fmi2OK, "LogAll", format, args);
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
extern "C" fmi2Component fmi2Instantiate(fmi2String instanceName,
                                         fmi2Type fmuType, fmi2String fmuGUID,
                                         fmi2String fmuResourceLocation,
                                         const fmi2CallbackFunctions* functions,
                                         fmi2Boolean visible,
                                         fmi2Boolean loggingOn) {
  // do not return null
  if (g_clients.size() == 0) {
    g_clients.push_back(NULL);  // Dummy var
  }

  currentId++;

  if (loggingOn) {
    LOG(functions, instanceName, fmi2OK, "logFmiCall",
        "FMU: Called instantiate with instance %s and GUID %s", instanceName,
        fmuGUID);
  }
  // std::string shared_memory_key(fmuGUID);
  // shared_memory_key.append(instanceName);
  // replace key with hash
  //	shared_memory_key =
  // std::to_string(std::hash<std::string>()(shared_memory_key));
  char buf[UUID4_LEN];
  if (uuid4_generate(buf) != UUID4_ESUCCESS) {
    LOG(functions, instanceName, fmi2Fatal, "logFmiCall",
        "FMU: Could not generate UUID4 for shared memory%s", "");
  }
  std::string shared_memory_key(buf);

  //	setbuf(stdout, NULL); //fixme remove

  std::string resourceLocationStr(URIToNativePath(fmuResourceLocation));

  if (loggingOn) {
    LOG(functions, instanceName, fmi2OK, "logFmiCall",
        "FMU: Launching Tool Wrapper memory key: '%s' and resource location %s",
        shared_memory_key.c_str(), resourceLocationStr.c_str());
  }

  std::string configFile = resourceLocationStr + std::string("/config.txt");
  ConfigFile config(configFile, shared_memory_key);

  JavaLauncher::debug = false;
  JavaLauncher* launcher =
      new JavaLauncher(visible, resourceLocationStr.c_str(), config.m_args);

  if (config.m_skipLaunch) {
    LOG(functions, instanceName, fmi2OK, "logFmiCall",
        "FMU: FMU Debug skipping launch of external FMU. SHM is '%s'",
        shared_memory_key.c_str());
    shared_memory_key = "shmFmiTest";
  }

  if (loggingOn) {
    LOG(functions, instanceName, fmi2OK, "logFmiCall",
        "FMU: Launching with shared memory key: '%s'",
        shared_memory_key.c_str());
  }

  std::string debugFlagFile(resourceLocationStr);
  debugFlagFile.append("/DEBUG");

  bool DEBUG = FileExists(debugFlagFile);

  FmuProxy* client = new FmuProxy(currentId, shared_memory_key);
  if (!DEBUG) {
    client->getChannel()->debugPrintPtr = NULL;
  } else {
    client->getChannel()->enableConsoleDebug();
  }
  FmuContainer* container =
      new FmuContainer(currentId, client, instanceName, functions, launcher);

  g_clients.push_back(container);

  if (!client->initialize()) {
    LOG(functions, instanceName, fmi2Fatal, "logFmiCall",
        "FMU: FMU Debug FATAL: cannot create and initialize IPC server for "
        "FMU. SHM is '%s'\n",
        shared_memory_key.c_str());
    return NULL;
  }

  if (loggingOn) {
    LOG(functions, instanceName, fmi2OK, "logFmiCall",
        "FMU: FMU Server create, hosting SHM with raw key: %s",
        shared_memory_key.c_str());
  }

  //	fflush(stdout); //FIXME remove

  if (!config.m_skipLaunch) {
    launcher->launch();
  }

  const char* callbackTag = "Callback";
  char* threadCallbackId =
      (char*)calloc(sizeof(char), strlen(fmuGUID) + strlen(instanceName) +
                                      strlen(callbackTag) + 1);
  strcpy(threadCallbackId, fmuGUID);
  strcat(threadCallbackId, instanceName);
  strcat(threadCallbackId, callbackTag);

  if (client->fmi2Instantiate(instanceName, fmuGUID, fmuResourceLocation,
                              threadCallbackId, visible, loggingOn)) {
    // connected
    if (functions != NULL && functions->logger != NULL && loggingOn) {
      // configure callback
      std::thread* callbackThread =
          new std::thread(callbackThreadFunc, container, threadCallbackId);
      container->callbackThread = callbackThread;
    }
  } else {
    // initialization failed
    ::fmi2FreeInstance((fmi2Component)container);
    return NULL;
  }

  return (void*)container;
}

extern "C" fmi2Status fmi2SetupExperiment(
    fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance,
    fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2SetupExperiment(
        toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime));
  }

  return fmi2OK;
}

extern "C" fmi2Status fmi2EnterInitializationMode(fmi2Component c) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2EnterInitializationMode());
  }

  return fmi2Fatal;
}

extern "C" fmi2Status fmi2ExitInitializationMode(fmi2Component c) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2ExitInitializationMode());
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2Terminate(fmi2Component c) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2Terminate());
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2Reset(fmi2Component c) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2Reset());
  }
  return fmi2Fatal;
}

extern "C" void fmi2FreeInstance(fmi2Component c) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    fmu->m_proxy->fmi2FreeInstance();
    fmu->active = false;
    if (fmu->callbackThread != NULL) {
      fmu->callbackThread->join();
    }
    for (auto it = begin(g_clients); it != end(g_clients); ++it) {
      if (*it == fmu) {
        g_clients.erase(it);
        break;
      }
    }
    delete fmu;
  }
}

// ---------------------------------------------------------------------------
// FMI functions: class methods not depending of a specific model instance
// ---------------------------------------------------------------------------

extern "C" const char* fmi2GetVersion() { return fmi2Version; }

extern "C" const char* fmi2GetTypesPlatform() { return fmi2TypesPlatform; }

// ---------------------------------------------------------------------------
// FMI functions: logging control, setters and getters for Real, Integer,
// Boolean, String
// ---------------------------------------------------------------------------

extern "C" fmi2Status fmi2SetDebugLogging(fmi2Component c,
                                          fmi2Boolean loggingOn,
                                          size_t nCategories,
                                          const fmi2String categories[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    for (int i = 0; i < nCategories; i++) {
      if (strcmp(categories[i], "logShm") == 0) {
        if (loggingOn) {
          fmu->m_proxy->getChannel()->debugPrintPtr = &fmuInternalDebugPrint;
        }
      } else if (strcmp(categories[i], "logLaunch") == 0) {
        if (loggingOn) {
          fmu->m_proxy->getChannel()->debugPrintPtr = &fmuInternalDebugPrint;
        }
      }
    }

    return convertStatus(
        fmu->m_proxy->fmi2SetDebugLogging(loggingOn, nCategories, categories));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetReal(fmi2Component c,
                                  const fmi2ValueReference vr[], size_t nvr,
                                  fmi2Real value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2GetReal(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetInteger(fmi2Component c,
                                     const fmi2ValueReference vr[], size_t nvr,
                                     fmi2Integer value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2GetInteger(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetBoolean(fmi2Component c,
                                     const fmi2ValueReference vr[], size_t nvr,
                                     fmi2Boolean value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2GetBoolean(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetString(fmi2Component c,
                                    const fmi2ValueReference vr[], size_t nvr,
                                    fmi2String value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2GetString(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetReal(fmi2Component c,
                                  const fmi2ValueReference vr[], size_t nvr,
                                  const fmi2Real value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2SetReal(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetInteger(fmi2Component c,
                                     const fmi2ValueReference vr[], size_t nvr,
                                     const fmi2Integer value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2SetInteger(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetBoolean(fmi2Component c,
                                     const fmi2ValueReference vr[], size_t nvr,
                                     const fmi2Boolean value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2SetBoolean(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2SetString(fmi2Component c,
                                    const fmi2ValueReference vr[], size_t nvr,
                                    const fmi2String value[]) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2SetString(vr, nvr, value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* FMUstate) {
  notimplemented(c, "fmi2GetFMUstate");
  return fmi2Error;
}
extern "C" fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate FMUstate) {
  notimplemented(c, "fmi2SetFMUstate");
  return fmi2Error;
}
extern "C" fmi2Status fmi2FreeFMUstate(fmi2Component c,
                                       fmi2FMUstate* FMUstate) {
  notimplemented(c, "fmi2FreeFMUstate");
  return fmi2Error;
}
extern "C" fmi2Status fmi2SerializedFMUstateSize(fmi2Component c,
                                                 fmi2FMUstate FMUstate,
                                                 size_t* size) {
  notimplemented(c, "fmi2SerializedFMUstateSize");
  return fmi2Error;
}
extern "C" fmi2Status fmi2SerializeFMUstate(fmi2Component c,
                                            fmi2FMUstate FMUstate,
                                            fmi2Byte serializedState[],
                                            size_t size) {
  notimplemented(c, "fmi2SerializeFMUstate");
  return fmi2Error;
}
extern "C" fmi2Status fmi2DeSerializeFMUstate(fmi2Component c,
                                              const fmi2Byte serializedState[],
                                              size_t size,
                                              fmi2FMUstate* FMUstate) {
  notimplemented(c, "fmi2DeSerializeFMUstate");
  return fmi2Error;
}

extern "C" fmi2Status fmi2GetDirectionalDerivative(
    fmi2Component c, const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
    const fmi2ValueReference vKnown_ref[], size_t nKnown,
    const fmi2Real dvKnown[], fmi2Real dvUnknown[]) {
  notimplemented(c, "fmi2GetDirectionalDerivative");
  return fmi2Error;
}

// ---------------------------------------------------------------------------
// Functions for FMI for Co-Simulation
// ---------------------------------------------------------------------------
#ifdef FMI_COSIMULATION
/* Simulating the slave */
extern "C" fmi2Status fmi2SetRealInputDerivatives(fmi2Component c,
                                                  const fmi2ValueReference vr[],
                                                  size_t nvr,
                                                  const fmi2Integer order[],
                                                  const fmi2Real value[]) {
  notimplemented(c, "fmi2SetRealInputDerivatives");
  return fmi2Error;
}

extern "C" fmi2Status fmi2GetRealOutputDerivatives(
    fmi2Component c, const fmi2ValueReference vr[], size_t nvr,
    const fmi2Integer order[], fmi2Real value[]) {
  notimplemented(c, "fmi2GetRealOutputDerivatives");
  return fmi2Error;
}

extern "C" fmi2Status fmi2CancelStep(fmi2Component c) {
  notimplemented(c, "fmi2CancelStep");
  return fmi2Error;
}

extern "C" fmi2Status fmi2DoStep(fmi2Component c,
                                 fmi2Real currentCommunicationPoint,
                                 fmi2Real communicationStepSize,
                                 fmi2Boolean noSetFMUStatePriorToCurrentPoint) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2DoStep(
        currentCommunicationPoint, communicationStepSize,
        noSetFMUStatePriorToCurrentPoint));
  }

  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s,
                                    fmi2Status* value) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    FmuProxy::fmi2Status status;
    FmuProxy::fmi2StatusKind kind = convertStatusKind(s);
    fmi2Status s = convertStatus(fmu->m_proxy->fmi2GetStatus(kind, &status));
    *value = convertStatus(status);
    return s;
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s,
                                        fmi2Real* value) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(
        fmu->m_proxy->fmi2GetRealStatus(convertStatusKind(s), value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetIntegerStatus(fmi2Component c,
                                           const fmi2StatusKind s,
                                           fmi2Integer* value) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(
        fmu->m_proxy->fmi2GetIntegerStatus(convertStatusKind(s), value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetBooleanStatus(fmi2Component c,
                                           const fmi2StatusKind s,
                                           fmi2Boolean* value) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(
        fmu->m_proxy->fmi2GetBooleanStatus(convertStatusKind(s), value));
  }
  return fmi2Fatal;
}

extern "C" fmi2Status fmi2GetStringStatus(fmi2Component c,
                                          const fmi2StatusKind s,
                                          fmi2String* value) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(
        fmu->m_proxy->fmi2GetStringStatus(convertStatusKind(s), value));
  }
  return fmi2Fatal;
}

/* INTO cps specific*/
extern "C" fmi2Status fmi2GetMaxStepsize(fmi2Component c, fmi2Real* size) {
  FmuContainer* fmu = getFmuContainer(c);

  if (fmu != NULL) {
    return convertStatus(fmu->m_proxy->fmi2GetMaxStepsize(size));
  }

  return fmi2Fatal;
}

// ---------------------------------------------------------------------------
// Functions for FMI2 for Model Exchange
// ---------------------------------------------------------------------------
#else
/* Enter and exit the different modes */
fmi2Status fmi2EnterEventMode(fmi2Component c) { return fmi2Error; }

fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo* eventInfo) {
  return fmi2Error;
}

fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c) { return fmi2Error; }

fmi2Status fmi2CompletedIntegratorStep(
    fmi2Component c, fmi2Boolean noSetFMUStatePriorToCurrentPoint,
    fmi2Boolean* enterEventMode, fmi2Boolean* terminateSimulation) {
  return fmi2Error;
}

/* Providing independent variables and re-initialization of caching */
fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time) { return fmi2Error; }

fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[],
                                   size_t nx) {
  return fmi2Error;
}

/* Evaluation of the model equations */
fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[],
                              size_t nx) {
  return fmi2Error;
}

fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[],
                                  size_t ni) {
  return fmi2Error;
}

fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real states[],
                                   size_t nx) {
  return fmi2Error;
}

fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c,
                                             fmi2Real x_nominal[], size_t nx) {
  return fmi2Error;
}
#endif  // Model Exchange
