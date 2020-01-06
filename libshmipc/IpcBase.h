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

#ifndef IPCBASE_H_
#define IPCBASE_H_

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <tchar.h>
typedef HANDLE SIGNAL_HANDLE;
#define strdup (const char*) _strdup;
#elif __APPLE__ || __linux
#include <unistd.h>
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
#include <sys/types.h>
#include <time.h>
#include "sem_timedwait.h"

#include "SharedFmiMessage.h"
using namespace sharedfmimemory;


namespace FmiIpc {

#define dprintf(format, args...) \
  if (this->debugPrintPtr) this->debugPrintPtr(this->m_id, format, args);
#define dprint(format) \
  if (this->debugPrintPtr) this->debugPrintPtr(this->m_id, format);

class IpcBase {
 public:
  IpcBase(int id, const char* shmName);
  virtual ~IpcBase();

  typedef int debugPrintType(int sender, const char* format, ...);

  static const char* SIGNAL_NAME;
  static const char* SIGNAL_AVALIABLE_NAME;
  static const char* SHARED_MEM_BASE_NAME;
  static const char* SIGNAL_CONN_WATCH_DOG_NAME;
  debugPrintType* debugPrintPtr;

  int getId() { return this->m_id; };
  void enableConsoleDebug();

 private:
  static int internalDebugPrint(int sender, const char* format, ...);
  static void signalClose(SIGNAL_HANDLE signal);
  void unmap(void* ptr, std::string* name);

 protected:
  void mapShm(bool* success, HANDLE handle, bool truncate);
  HANDLE openShm(bool* success, const char* name, bool create = false);
  bool signalWait(SIGNAL_HANDLE signal, DWORD dwTimeout);
  void signalPost(SIGNAL_HANDLE signal);
  SIGNAL_HANDLE createSignal(bool* success, const char* baseName,
                             bool create = false);

  void sleep(int milliseconds);

 protected:
  int m_id;
  // Internal variables
  HANDLE m_hMapFile;  // Handle to the mapped memory file
  const char* m_hMapFileName;
  SIGNAL_HANDLE m_hSignal;  // Event used to signal when data exists
  SIGNAL_HANDLE
  m_hAvail;  // Event used to signal when some blocks become available
  SIGNAL_HANDLE m_hConnWatchDogSignal;  // Event used to signal alive
  std::string* m_name;
  SharedFmiMem* m_pBuf;  // Buffer that points to the shared memory

  const char* m_log_name_id;

  //		int printf(int sender, const char * format, ...);
  std::string getMappedName(void* self, const char* baseName, const char* name);
};

} /* namespace FmiIpc */

#endif /* IPCBASE_H_ */
