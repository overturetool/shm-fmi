#ifndef SHAREDFMIMEMORY_H_
#define SHAREDFMIMEMORY_H_

#ifdef _WIN32
#elif __APPLE__ ||  __linux
//POSIX
#include <semaphore.h>
#endif

//#include "fmi2FunctionTypes.h"
namespace sharedfmimemory
{

typedef enum
	:int
	{
		fmi2SetDebugLogging = 0, fmi2Instantiate = 1,
//	fmi2FreeInstance,
	fmi2SetupExperiment = 2,
	fmi2EnterInitializationMode = 3,
	fmi2ExitInitializationMode = 4,
	fmi2Terminate = 5,
	fmi2Reset = 6,
	fmi2DoStep = 7,

	fmi2GetReal = 8,
	fmi2GetInteger = 9,
	fmi2GetBoolean = 10,
	fmi2GetString = 11,

	fmi2SetReal = 12,
	fmi2SetInteger = 13,
	fmi2SetBoolean = 14,
	fmi2SetString = 15,

	fmi2GetMaxStepSize = 16,

	//status
	fmi2GetStatus=17,
	fmi2GetRealStatus=18,
	fmi2GetIntegerStatus=19,
	fmi2GetBooleanStatus=20,
	fmi2GetStringStatus=21,
	//
	fmi2Log=22

} fmi2Command;

#define SharedFmiBufferSize 1000
//#define StringMaxSize 300
/*
 struct SharedFmiMessageData {
 int size;
 fmi2ValueReference valueReferences[SharedFmiBufferSize];
 fmi2Real reals[SharedFmiBufferSize];
 fmi2Integer integers[SharedFmiBufferSize];
 fmi2Boolean booleans[SharedFmiBufferSize];
 fmi2String stringsSharedFmiBufferSize[SharedFmiBufferSize][StringMaxSize];

 unsigned char protoBufMsg[3000];

 };
 */
struct SharedFmiMessage
{

	fmi2Command cmd;
	//SharedFmiMessageData data;
	unsigned char protoBufMsg[SharedFmiBufferSize];
	int protoBufMsgSize;
};

/*
 struct SharedFmiMessageReply {
 fmi2Command cmd;
 fmi2Status status;
 fmi2String stringsSharedFmiBufferSize[SharedFmiBufferSize][StringMaxSize];
 };
 */
struct SharedFmiMem
{
	SharedFmiMessage message;
//	SharedFmiMessageReply reply;
#ifdef _WIN32
#elif __APPLE__ ||  __linux
//POSIX
	sem_t semSignal;
	sem_t semAvail;
#endif

};

}
#endif /* SHAREDFMIMEMORY_H_ */
