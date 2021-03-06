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

#include "IpcBase.h"

#include <stdio.h>
#include <cstdio>
#include <stddef.h>
#include <string>

namespace FmiIpc {

#ifdef _WIN32
const char* IpcBase::SHARED_MEM_BASE_NAME = "Local\\";
const char* IpcBase::SIGNAL_AVALIABLE_NAME = "Local\\sigAvail";
const char* IpcBase::SIGNAL_NAME = "Local\\sig";
const char* IpcBase::SIGNAL_CONN_WATCH_DOG_NAME = "Local\\connWatchDog";

#elif __APPLE__ || __linux
// Apple must start with / and not have \\ and have a max size incl NULL of 32
// Linux must start with / and not have any slashes after that
const char* IpcBase::SHARED_MEM_BASE_NAME = "/Local";
const char* IpcBase::SIGNAL_AVALIABLE_NAME = "/sigAvail";
const char* IpcBase::SIGNAL_NAME = "/sig";
const char* IpcBase::SIGNAL_CONN_WATCH_DOG_NAME = "/connWatchDog";
#endif

IpcBase::IpcBase(int id, const char* shmName) {
  this->debugPrintPtr = NULL;  // &IpcBase::internalDebugPrint;

  this->m_id = id;
  this->m_name = new std::string(shmName);

  m_hMapFile = 0;
  m_hMapFileName = NULL;
  m_hSignal = 0;
  m_hAvail = 0;
  m_pBuf = NULL;
  m_log_name_id = NULL;
  m_hConnWatchDogSignal = NULL;
}

IpcBase::~IpcBase() {
  // Close the event
  if (m_hSignal) {
    dprintf("\tIPC %s %d: Closing m_hSignal for: '%s'\n", m_log_name_id, m_id,
            m_name->c_str());
    signalClose(m_hSignal);
    m_hSignal = NULL;
  }

  // Close the event
  if (m_hAvail) {
    dprintf("\tIPC %s %d: Closing m_hAvail for: '%s'\n", m_log_name_id, m_id,
            m_name->c_str());
    signalClose(m_hAvail);
    m_hAvail = NULL;
  }

  // Close the event
  if (m_hConnWatchDogSignal) {
    dprintf("\tIPC %s %d: Closing m_hConnWatchDogSignal for: '%s'\n",
            m_log_name_id, m_id, m_name->c_str());
    signalClose(m_hConnWatchDogSignal);
    m_hConnWatchDogSignal = NULL;
  }

  // Unmap the memory
  if (m_pBuf) {
    dprintf("\tIPC %s %d: Unmapping m_pBuf for: '%s'\n", m_log_name_id, m_id,
            m_name->c_str());
    unmap(m_pBuf, m_name);
    m_pBuf = NULL;
  }

  // Close the file handle
  if (m_hMapFile) {
    dprintf("\tIPC %s %d: Unlink m_hMapFile for: '%s'\n", m_log_name_id, m_id,
            m_hMapFileName);

#ifdef _WIN32
    CloseHandle(m_hMapFile);
#elif __APPLE__ || __linux
    // POSIX close handle
    close(m_hMapFile);

    // POSIX remove shared memory
    shm_unlink(m_hMapFileName);
#endif

    m_hMapFile = (HANDLE)NULL;
  }

  if (this->m_name) {
    delete this->m_name;
  }
}

void IpcBase::enableConsoleDebug() {
  this->debugPrintPtr = &IpcBase::internalDebugPrint;
}

int IpcBase::internalDebugPrint(int sender, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int ret = vfprintf(stdout, format, args);
  va_end(args);
  return ret;
}

/**
 * This function create a semaphore or shared memory name
 */
std::string IpcBase::getMappedName(void* self, const char* baseName,
                                   const char* name) {
  std::string nameOfMapping(baseName);
  nameOfMapping += std::string(name);
//  dprintf("\tIPC %s: Creating mapped name from '%s' and '%s' =
//  '%s'\n",m_log_name_id, baseName, name,
//          nameOfMapping.c_str());
#ifdef __APPLE__

  if (nameOfMapping.length() >= 29)  // 31 is max incl NULL
  {
    std::string hash = std::to_string(std::hash<std::string>()(nameOfMapping));
    nameOfMapping = "/";
    nameOfMapping += hash;
    //    dprintf("\tIPC %s: New Apple shm key is: %s from baseName %s and name
    //    %s\n",m_log_name_id,
    //            nameOfMapping.c_str(), baseName, name);
  }

#endif

  return nameOfMapping;
}

#ifdef _WIN32
// Returns the last Win32 error, in string format. Returns an empty string if
// there is no error.
static std::string GetLastErrorAsString() {
  // Get the error message, if any.
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0)
    return std::string();  // No error message has been recorded

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  // Free the buffer.
  LocalFree(messageBuffer);

  return message;
}
#endif

HANDLE IpcBase::openShm(bool* success, const char* name, bool create) {
  HANDLE handle = (HANDLE)NULL;
  if (create) {
// create

#ifdef _WIN32
    handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
                               sizeof(SharedFmiMem), name);

#elif __APPLE__
    // POSIX
    shm_unlink(name);
    handle = shm_open(name, O_CREAT | O_RDWR, ACCESSPERMS);

#elif __linux
    handle = shm_open(name, O_CREAT | O_TRUNC | O_RDWR, ACCESSPERMS);
#endif

  } else {
// open
#ifdef _WIN32
    handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,  // read/write access
                             FALSE,                // do not inherit the name
                             name);                // name of mapping object
#elif __APPLE__ || __linux
    // POSIX
    handle = shm_open(name, O_RDWR, 0666);
#endif
  }

#ifdef _WIN32
  if (handle == NULL || handle == INVALID_HANDLE_VALUE) {
    dprintf("\tIPC %s %d: Failed to create/open shm: %s\n", m_log_name_id, m_id,
            GetLastErrorAsString().c_str());
    *success = false;
  }
#else
  if (handle == -1) {
    dprintf("\tIPC %s %d: server_create: failed: %01d %s\n", m_log_name_id,
            m_id, __LINE__, strerror(errno));
    *success = false;
  }
#endif
  return handle;
}

/**
 * Function to map shm handle to local memory
 */
void IpcBase::mapShm(bool* success, HANDLE handle, bool truncate) {
// create
#ifdef _WIN32
  m_pBuf = (SharedFmiMem*)MapViewOfFile(
      handle,               // handle to map object
      FILE_MAP_ALL_ACCESS,  // read/write permission
      0, 0, sizeof(SharedFmiMem));
  if (m_pBuf == NULL) {
    dprintf("\tIPC %s %d: MapViewOfFile: failed: %01d\n", m_log_name_id, m_id,
            __LINE__);
    *success = false;
    return;
  }

  // truncate
  if (truncate) {
    // Clear the buffer
    ZeroMemory(m_pBuf, sizeof(SharedFmiMem));
  }

#elif __APPLE__ || __linux
  if (truncate) {
    if (ftruncate(handle, sizeof(SharedFmiMem)) != 0) {
      dprintf("\tIPC %s %d: ftruncate: failed: unable to truncate. %s\n",
              m_log_name_id, m_id, strerror(errno));
      *success = false;
      return;
    }
  }

  void* ptr = mmap(NULL, sizeof(SharedFmiMem), PROT_READ | PROT_WRITE,
                   MAP_SHARED, handle, 0);
  if (ptr == MAP_FAILED) {
    dprintf("\tIPC %s %d: mmap: failed: mmap: %s\n", m_log_name_id, m_id,
            strerror(errno));
    *success = false;
    return;
  }

  m_pBuf = (SharedFmiMem*)ptr;

#endif
}

/**
 *
 * This function creates or opens a named semaphore
 *
 */
SIGNAL_HANDLE IpcBase::createSignal(bool* success, const char* baseName,
                                    bool create) {
  std::string signalName = getMappedName(this, baseName, this->m_name->c_str());
  SIGNAL_HANDLE signal = NULL;
#ifdef _WIN32
  DWORD security = SYNCHRONIZE | EVENT_MODIFY_STATE;
  if (create) {
    security |= DELETE;
  }

  signal = OpenEventA(security, FALSE, signalName.c_str());
  if (signal == NULL) {
    if (!create) {
      dprintf("\tIPC %s %d: OpenEventA failed. Error: %s\n", m_log_name_id,
              m_id, GetLastErrorAsString().c_str());
    }
  }
  if (create) {
    if (signal != NULL) {
      CloseHandle(signal);
    }

    // Create the events
    signal = CreateEventA(NULL, FALSE, FALSE, signalName.c_str());
    if (signal == NULL) {
      dprintf("\tIPC %s %d: CreateEventA failed. Error: %s\n", m_log_name_id,
              m_id, GetLastErrorAsString().c_str());
      *success = false;
    }
  }
  if (signal == NULL || signal == INVALID_HANDLE_VALUE) {
    if (signal == INVALID_HANDLE_VALUE) {
      dprintf("\tIPC %s %d: signal_create: invalid handle\n", m_log_name_id,
              m_id);
    }
    dprintf("signal_create: failed: %01d\n", __LINE__);
    *success = false;
  }

#elif __APPLE__ || __linux__

  if (create) {
    dprintf("\tIPC %s %d: creating new signal: '%s'\n", m_log_name_id, m_id,
            signalName.c_str());
    sem_unlink(signalName.c_str());
    signal = sem_open(signalName.c_str(), O_CREAT, ALLPERMS, 0);

  } else {
    dprintf("\tIPC %s %d: creating new signal: '%s'\n", m_log_name_id, m_id,
            signalName.c_str());
    signal = sem_open(signalName.c_str(), 0);
  }

  if (signal == SEM_FAILED) {
    dprintf("\tIPC %s %d: signal_create: failed: sem_open signal: %s\n",
            m_log_name_id, m_id, strerror(errno));
    *success = false;
  }

#endif

  if (!success) {
    signal = NULL;
  }

  return signal;
}

bool IpcBase::signalWait(SIGNAL_HANDLE signal, DWORD dwTimeout) {
// Wait on the available event

#ifdef _WIN32
  if (dwTimeout == 0) {
    dwTimeout = INFINITE;
  }
  if (WaitForSingleObject(signal, dwTimeout) != WAIT_OBJECT_0) return false;
#elif __APPLE__ || __linux__

  if (dwTimeout == 0) {
    if (sem_wait(signal) == -1) {
      return false;
    }
  } else {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec = ts.tv_sec + (dwTimeout / 1000);

    ts.tv_sec = ts.tv_sec + (dwTimeout / 1000);
    ts.tv_nsec = ts.tv_nsec + ((dwTimeout % 1000) * 1000000);

    if (sem_timedwait(signal, &ts) == -1) {
      return false;
    }
  }
#endif

  // Success
  return true;
}

void IpcBase::signalPost(SIGNAL_HANDLE signal) {
#ifdef _WIN32
  SetEvent(signal);
#elif __APPLE__ || __linux

  if (sem_post(signal) == -1) {
    dprintf("\tIPC %s %d: signal: failed: sem_postn signal 'm_hAvail': %s\n",
            m_log_name_id, m_id, strerror(errno));
  }
#endif
}

void IpcBase::signalClose(SIGNAL_HANDLE signal) {
#ifdef _WIN32
  CloseHandle(signal);
#elif __APPLE__ || __linux
  // POSIX
  sem_close(signal);
#endif
}

void IpcBase::unmap(void* ptr, std::string* name) {
#ifdef _WIN32
  UnmapViewOfFile(ptr);
#elif __APPLE__ || __linux
  // POSIX
  int r = munmap(ptr, sizeof(SharedFmiMem));
  if (r != 0) {
    dprintf("\tIPC %s %d: munmap", m_log_name_id, m_id);
  }
#endif
}

void IpcBase::sleep(int milliseconds)  // cross-platform sleep function
{
#ifdef WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds * 1000);
#endif  // win32
}

} /* namespace FmiIpc */
