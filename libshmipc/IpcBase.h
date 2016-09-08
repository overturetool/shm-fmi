/*
 * IpcBase.h
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#ifndef IPCBASE_H_
#define IPCBASE_H_

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <tchar.h>
typedef HANDLE SIGNAL_HANDLE;
#elif __APPLE__ ||  __linux
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef int HANDLE;
typedef bool BOOL;
#define INFINITE 0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
typedef sem_t* SIGNAL_HANDLE;
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string>

#include "SharedFmiMessage.h"
using namespace sharedfmimemory;

// Definitions
#define IPC_BLOCK_COUNT			512
#define IPC_BLOCK_SIZE			4096

#define IPC_MAX_ADDR			256

namespace FmiIpc
{

#define dprintf(format,args...) if(this->debugPrintPtr) this->debugPrintPtr(this->id,format,args);
#define dprint(format) if(this->debugPrintPtr) this->debugPrintPtr(this->id,format);

class IpcBase
{
public:
	IpcBase(int id, const char* shmName);
	virtual ~IpcBase();

	typedef int debugPrintType(int sender, const char * format, ...);



	static const char* SIGNAL_NAME;
	static const char* SIGNAL_AVALIABLE_NAME;
	static const char* SHARED_MEM_BASE_NAME;
	 debugPrintType *debugPrintPtr;

	static void close(HANDLE handle)
	{
#ifdef _WIN32
		CloseHandle(handle);
#elif __APPLE__ ||  __linux
//POSIX
		close(handle);
#endif
	}

#ifdef __APPLE__
	static void close(SIGNAL_HANDLE handle)
	{
		sem_close(handle);
	}
#elif __linux
	static void close(SIGNAL_HANDLE handle)
	{
		sem_close(handle);
	}
#endif

	 void unmap(void* ptr, std::string* name)
	{
#ifdef _WIN32
		UnmapViewOfFile(ptr);
#elif __APPLE__ ||  __linux
		//POSIX
		int r = munmap(ptr, sizeof(SharedFmiMem));
		if (r != 0)
		{
			dprint("munmap");

		}
#endif
	}

	 int getId(){return this->id;};
	 void enableConsoleDebug();

private:
	static int internalDebugPrint(int sender, const char * format, ...);
protected:
	void mapShm(bool*success,HANDLE handle, bool truncate);
	HANDLE openShm(bool*success,const char* name,bool create = false);

protected:
	int id;
		// Internal variables
		HANDLE m_hMapFile;		// Handle to the mapped memory file
		SIGNAL_HANDLE m_hSignal;		// Event used to signal when data exists
		SIGNAL_HANDLE m_hAvail;		// Event used to signal when some blocks become available
		std::string* m_name;
		SharedFmiMem *m_pBuf;		// Buffer that points to the shared memory

//		int printf(int sender, const char * format, ...);
	std::string getMappedName(void* self, const char* baseName, const char* name);

	SIGNAL_HANDLE createSignal(const char* baseName, bool create=false);
};

} /* namespace FmiIpc */

#endif /* IPCBASE_H_ */
