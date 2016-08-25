/*
 * IpcBase.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcBase.h"

#include <stdio.h>
#include <cstdio>
#include <stddef.h>
#include <string>

namespace FmiIpc
{

#ifdef _WIN32
const char* IpcBase::SHARED_MEM_BASE_NAME = "Local\\";
const char* IpcBase::SIGNAL_AVALIABLE_NAME = "sigAvail";
const char* IpcBase::SIGNAL_NAME = "sig";
#elif __APPLE__ ||  __linux
//Apple must start with / and not have \\ and have a max size incl NULL of 32
//Linux must start with / and not have any slashes after that
const char* IpcBase::SHARED_MEM_BASE_NAME = "/Local";
const char* IpcBase::SIGNAL_AVALIABLE_NAME = "/sigAvail";
const char* IpcBase::SIGNAL_NAME = "/sig";
#endif

IpcBase::IpcBase(int id, const char* shmName)
{
	this->debugPrintPtr = NULL; // &IpcBase::internalDebugPrint;

	this->id = id;
	this->m_name = new std::string(shmName);

	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
}

IpcBase::~IpcBase()
{
	// Close the event
	if (m_hSignal)
	{
		SIGNAL_HANDLE handle = m_hSignal;
		m_hSignal = 0;
		close(handle);
	}

	// Close the event
	if (m_hAvail)
	{
		SIGNAL_HANDLE handle = m_hAvail;
		m_hAvail = 0;
		close(handle);
	}

	// Unmap the memory
	if (m_pBuf)
	{
		SharedFmiMem *pBuff = m_pBuf;
		m_pBuf = NULL;
		//UnmapViewOfFile(pBuff);
		unmap(m_pBuf, m_name);
	}

	// Close the file handle
	if (m_hMapFile)
	{

#ifdef _WIN32
		close(m_hMapFile);
#elif __APPLE__ ||  __linux
		//POSIX
		shm_unlink(m_name->c_str());
#endif

		m_hMapFile = (HANDLE) NULL;
	}

	if (this->m_name)
	{
		delete this->m_name;
	}
}

void IpcBase::enableConsoleDebug()
{
	this->debugPrintPtr = &IpcBase::internalDebugPrint;
}


int IpcBase::internalDebugPrint(int sender, const char * format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vfprintf(stdout, format, args);
	va_end(args);
	return ret;
}

/**
 * This function create a semaphor or shared memory name
 */
std::string* IpcBase::getMappedName(void* self, const char* baseName, const char* name)
{
	std::string* nameOfMapping = new std::string(baseName);
	*nameOfMapping += std::string(name);
	dprintf("Creating signal name from '%s' and '%s' = '%s'\n", baseName, name, nameOfMapping->c_str());
#ifdef __APPLE__

	if (nameOfMapping->length() >= 29) //31 is max incl NULL
	{

		std::string hash = std::to_string(std::hash<std::string>()(*nameOfMapping));
		*nameOfMapping = "/";
		*nameOfMapping += hash;
		dprintf("New Apple shm key is: %s from baseName %s and name %s\n", nameOfMapping->c_str(), baseName, name);
	}

#endif

	return nameOfMapping;
}

#ifdef _WIN32
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
static std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0)
	return std::string();//No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
#endif

HANDLE IpcBase::openShm(bool*success, const char* name, bool create)
{
	HANDLE handle = (HANDLE) NULL;
	if (create)
	{
		//create

#ifdef _WIN32
		handle = CreateFileMapping( INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE, 0, sizeof(SharedFmiMem), name);

#elif __APPLE__
		//POSIX
		shm_unlink(name);
		handle = shm_open(name, O_CREAT | O_RDWR, ACCESSPERMS);

#elif  __linux
		handle = shm_open(name, O_CREAT | O_TRUNC | O_RDWR,ACCESSPERMS);
#endif

	} else
	{
		//open
#ifdef _WIN32
		handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,	// read/write access
				FALSE,// do not inherit the name
				name);// name of mapping object
#elif __APPLE__ ||  __linux
		//POSIX
		handle = shm_open(name, O_RDWR, 0666);
#endif
	}

#ifdef _WIN32
	if (handle == NULL || handle == INVALID_HANDLE_VALUE)
	{
		dprintf("Failed to create/open shm: %s\n",GetLastErrorAsString().c_str());
		*success=false;
	}
#else
	if (handle == -1)
	{
		dprintf("server_create: failed: %01d %s\n", __LINE__, strerror( errno));
		*success = false;
	}
#endif
	return handle;
}

/**
 * Function to map shm handle to local memory
 */
void IpcBase::mapShm(bool*success, HANDLE handle, bool truncate)
{
	*success = true;
	//create
#ifdef _WIN32
	m_pBuf = (SharedFmiMem*) MapViewOfFile(handle,	// handle to map object
			FILE_MAP_ALL_ACCESS,// read/write permission
			0, 0, sizeof(SharedFmiMem));
	if (m_pBuf == NULL)
	{
		dprintf("MapViewOfFile: failed: %01d\n", __LINE__);
		*success =false;
		return;
	}

	//truncate
	if (truncate)
	{
		// Clear the buffer
		ZeroMemory(m_pBuf, sizeof(SharedFmiMem));
	}

#elif __APPLE__ ||  __linux
	if (truncate)
	{
		if (ftruncate(handle, sizeof(SharedFmiMem)) != 0)
		{
			dprintf("ftruncate: failed: unable to truncate. %s\n", strerror( errno));
			*success = false;
			return;
		}
	}

	void *ptr = mmap(NULL, sizeof(SharedFmiMem), PROT_READ | PROT_WRITE, MAP_SHARED, handle, 0);
	if (ptr == MAP_FAILED)
	{
		dprintf("mmap: failed: mmap: %s\n", strerror( errno));
		*success = false;
		return;
	}

	m_pBuf = (SharedFmiMem*) ptr;

#endif

}

/**
 *
 * This function creates or opens a named semaphore
 *
 */
SIGNAL_HANDLE IpcBase::createSignal(const char* baseName, bool create)
{
	std::string* signalName = getMappedName(this, baseName, this->m_name->c_str());
	bool success = true;
	SIGNAL_HANDLE signal = NULL;
#ifdef _WIN32

	// Create the events
	signal = CreateEventA(NULL, FALSE, FALSE, signalName->c_str());

	if (signal == NULL || signal == INVALID_HANDLE_VALUE)
	{
		dprintf("signal_create: failed: %01d\n", __LINE__);
		success = false;;
	}

#elif __APPLE__ || __linux__

	if (create)
	{
		dprintf("creating new signal: '%s'\n", signalName->c_str());
		sem_unlink(signalName->c_str());
		signal = sem_open(signalName->c_str(), O_CREAT, ALLPERMS, 0);

	} else
	{
		dprintf("creating new signal: '%s'\n", signalName->c_str());
		signal = sem_open(signalName->c_str(), 0);

	}

	if (signal == SEM_FAILED)
	{
		dprintf("signal_create: failed: sem_open signal: %s\n", strerror( errno));
		success = false;
	}

#endif

	delete signalName;
	if (!success)
	{
		signal = NULL;
	}

	return signal;
}

} /* namespace FmiIpc */
