/*
 * FmiIpc.cpp
 *
 *  Created on: 25/09/2015
 *      Author: kel
 */

#include "FmiIpc.h"

#include <stdio.h>
#include <cstdio>
#include <stddef.h>
#include <string>

#ifdef __APPLE__
//Apple must start with / and not have \\ and have a max size incl NULL of 32
const char* FmiIpc::SHARED_MEM_BASE_NAME = "/Local";
const char* FmiIpc::SIGNAL_AVALIABLE_NAME = "/sigAvail";
const char* FmiIpc::SIGNAL_NAME = "/sig";
#else
const char* FmiIpc::SHARED_MEM_BASE_NAME = "Local\\";
const char* FmiIpc::SIGNAL_AVALIABLE_NAME = "sigAvail";
const char* FmiIpc::SIGNAL_NAME = "sig";
#endif

//#define DEBUG

#if defined( DEBUG )
#define DEBUG_PRINTF(x) printf x
#else
#define DEBUG_PRINTF(x)
#endif

FmiIpc::FmiIpc()
{

}

FmiIpc::~FmiIpc()
{
}

std::string* getMappedName(const char* baseName, const char* name)
{
	std::string* nameOfMapping = new std::string(baseName);
	*nameOfMapping += std::string(name);

#ifdef __APPLE__

	if (nameOfMapping->length() >= 29) //31 is max incl NULL
	{

		std::string hash = std::to_string(std::hash<std::string>()(*nameOfMapping));
		*nameOfMapping = "/";
		*nameOfMapping += hash;
		printf("New Apple shm key is: %s from baseName %s and name %s\n", nameOfMapping->c_str(), baseName, name);
	}

#endif

	return nameOfMapping;
}

FmiIpc::Server::Server()
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
	m_name = NULL;
	// create the server
}

FmiIpc::Server::~Server()
{
	// Free memory
	// Close the server
	close();

	if (m_name != NULL)
	{
		delete m_name;
	}
}

void FmiIpc::Server::close()
{
	// Close the event
	if (m_hSignal)
	{
		SIGNAL_HANDLE handle = m_hSignal;
		m_hSignal = 0;
		FmiIpc::close(handle);
	}

	// Close the event
	if (m_hAvail)
	{
		SIGNAL_HANDLE handle = m_hAvail;
		m_hAvail = 0;
		FmiIpc::close(handle);
	}

	// Unmap the memory
	if (m_pBuf)
	{
		SharedFmiMem *pBuff = m_pBuf;
		m_pBuf = NULL;
		//UnmapViewOfFile(pBuff);
		FmiIpc::unmap(m_pBuf, m_name);
	}

	// Close the file handle
	if (m_hMapFile)
	{
		HANDLE handle = m_hMapFile;
		m_hMapFile = (HANDLE) NULL;

#ifdef _WIN32
		FmiIpc::close(m_hMapFile);
#elif __APPLE__ ||  __linux
//POSIX
		shm_unlink(m_name->c_str());
#endif

	}
}
;

bool FmiIpc::Server::create(const char* name)
{
	bool ok = true;
	std::string* nameOfMapping = getMappedName(SHARED_MEM_BASE_NAME, name);
	printf("Starting IPC server with key: %s\n", nameOfMapping->c_str());
	// Create the file mapping

#ifdef _WIN32
	m_hMapFile = CreateFileMapping( INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE, 0, sizeof(SharedFmiMem), nameOfMapping->c_str());
#elif __APPLE__ 
//POSIX
	shm_unlink(nameOfMapping->c_str());
	m_hMapFile = shm_open(nameOfMapping->c_str(), O_CREAT | O_RDWR, ALLPERMS);

#elif  __linux
	m_hMapFile = shm_open(nameOfMapping->c_str(), O_CREAT | O_TRUNC | O_RDWR,
			0666);
#endif

	this->m_name = nameOfMapping;

#ifdef _WIN32
	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE)
	{
#elif __APPLE__ ||  __linux
//POSIX
	if (m_hMapFile == -1)
	{
#endif

		switch (errno)
		{
		case EACCES:
			break;

		case EEXIST:
			break;
		case EINTR: //The shm_open() operation was interrupted by a signal.
			break;
		case EINVAL: //The shm_open() operation is not supported.
			break;
		case EMFILE:
			break;
		case ENAMETOOLONG:
			break;
		case ENFILE:
			break;
		case ENOENT:
			break;
		case ENOSPC:
			break;
		default:
			break;
		}

		printf("server_create: failed: %01d %s\n", __LINE__, strerror( errno));

		return false;
	}
	// Map to the file
#ifdef _WIN32
	m_pBuf = (SharedFmiMem*) MapViewOfFile(m_hMapFile,	// handle to map object
			FILE_MAP_ALL_ACCESS,// read/write permission
			0, 0, sizeof(SharedFmiMem));
	if (m_pBuf == NULL)
	{
		fprintf(stderr,"server_create: failed: %01d\n", __LINE__);
		return false;
	}
	// Clear the buffer
	ZeroMemory(m_pBuf, sizeof(SharedFmiMem));
#elif __APPLE__ ||  __linux
//POSIX

	if (ftruncate(m_hMapFile, sizeof(SharedFmiMem)) != 0)
	{

		switch (errno)
		{
		case EBADF:
			break;

		case EFBIG:
			break;
		case EINVAL:
			break;
		case EROFS:
			break;
		case EINTR:
			break;
		case EIO:
			break;
		default:
			break;
		}

		printf("server_create: failed: unable to truncate. %s\n", strerror( errno));
		return false;
	}

	void *ptr = mmap(NULL, sizeof(SharedFmiMem), PROT_READ | PROT_WRITE, MAP_SHARED, m_hMapFile, 0);
	if (ptr == MAP_FAILED)
	{

		switch (errno)
		{
		case EACCES:
			break;

		case EBADF:
			break;
		case EINVAL:
			break;
		case ENODEV:
			break;
		case ENOMEM:
			break;
		case ENXIO:
			break;
		case EOVERFLOW:
			break;
		default:
			break;
		}
		printf("server_create: failed: mmap: %s\n", strerror( errno));
		return false;
	}

	m_pBuf = (SharedFmiMem*) ptr;
#endif

	// Create the events
#ifdef _WIN32
	std::string* signalName = getMappedName(SIGNAL_NAME, name);
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, signalName->c_str());
	delete signalName;

	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE)
	{
		printf("server_create: failed: %01d\n", __LINE__);
		return false;
	}

	std::string* signalAvailName = getMappedName(SIGNAL_AVALIABLE_NAME, name);
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, signalAvailName->c_str());
	delete signalAvailName;

	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE)
	{
		printf("server_create: failed: %01d\n", __LINE__);
		return false;
	}
#elif __APPLE__ || __linux

	std::string* signalName = getMappedName(SIGNAL_NAME, name);
	sem_unlink(signalName->c_str());
	m_hSignal = sem_open(signalName->c_str(), O_CREAT, ALLPERMS, 0);

	if (m_hSignal == (sem_t *) SEM_FAILED)
	{
		printf("server_create: failed: sem_open signal: %s\n", strerror( errno));
		ok = false;

	}
	delete signalName;

	std::string* signalAvailName = getMappedName(SIGNAL_AVALIABLE_NAME, name);
	sem_unlink(signalAvailName->c_str());
	m_hAvail = sem_open(signalAvailName->c_str(), O_CREAT, ALLPERMS, 0);

	if (m_hAvail == (sem_t *) SEM_FAILED)
	{
		printf("server_create: failed: sem_open signal: %s\n", strerror( errno));
		ok = false;

	}

	delete signalAvailName;

#endif

	if (m_pBuf == NULL)
	{
		printf("Error in IPC server ctr, buf NULL\n");
		ok = false;
	}

	return ok;
}

SharedFmiMessage* FmiIpc::Server::send(SharedFmiMessage* message, DWORD dwTimeout)
{
	DEBUG_PRINTF(("IPC Server write msg\n"));
	this->m_pBuf->message = *message;

#ifdef _WIN32
	SetEvent(this->m_hAvail);
	DEBUG_PRINTF(("IPC Server signaled\n"));

	if (WaitForSingleObject(this->m_hSignal, dwTimeout) != WAIT_OBJECT_0)
	{
		return NULL;
	}
#elif __APPLE__ || __linux
	sem_post(this->m_hAvail);

	DEBUG_PRINTF(("IPC Server signaled\n"));

	sem_wait(this->m_hSignal);
#endif

	DEBUG_PRINTF(("IPC Server ret msg\n"));
	return &this->m_pBuf->message;
}

FmiIpc::Client::Client()
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
	m_name = NULL;
}

FmiIpc::Client::Client(const char* connectAddr, bool* success)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	printf("IPC Client key %s\n", connectAddr);

	std::string* nameOfMapping = getMappedName(SHARED_MEM_BASE_NAME, connectAddr);
	printf("Starting IPC client with key: %s\n", nameOfMapping->c_str());
	// Open the shared file

#ifdef _WIN32
	m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,	// read/write access
			FALSE,// do not inherit the name
			nameOfMapping->c_str());// name of mapping object
#elif __APPLE__ ||  __linux
//POSIX
	m_hMapFile = shm_open(nameOfMapping->c_str(), O_RDWR, 0666);
#endif

	this->m_name = nameOfMapping;

#ifdef _WIN32
	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE)
	{
#elif __APPLE__ ||  __linux
//POSIX
	if (m_hMapFile < 0)
	{
#endif

		//	printf("Starting client with key: %s - faild\n",				nameOfMapping->c_str());

		switch (errno)
		{
		case EACCES:
			break;

		case EEXIST:
			break;
		case EINTR:	//The shm_open() operation was interrupted by a signal.
			break;
		case EINVAL:	//The shm_open() operation is not supported.
			break;
		case EMFILE:
			break;
		case ENAMETOOLONG:
			break;
		case ENFILE:
			break;
		case ENOENT:
			break;
		case ENOSPC:
			break;
		default:
			break;
		}

		*success = false;
		return;
	}

	// Map to the file

#ifdef _WIN32
	m_pBuf = (SharedFmiMem*) MapViewOfFile(m_hMapFile,	// handle to map object
			FILE_MAP_ALL_ACCESS,// read/write permission
			0, 0, sizeof(SharedFmiMem));
	if (m_pBuf == NULL)
	{
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}

#elif __APPLE__ ||  __linux
//POSIX

	void *ptr = mmap(0, sizeof(SharedFmiMem), PROT_READ | PROT_WRITE,
	MAP_SHARED, m_hMapFile, 0);
	if (ptr == MAP_FAILED)
	{
		printf("server_create: failed: mmap\n");
		return;
	}

	m_pBuf = (SharedFmiMem*) ptr;
#endif

	if (m_pBuf == NULL)
	{
		*success = false;
		return;
	}

#ifdef _WIN32
	std::string* signalName = getMappedName(SIGNAL_NAME, connectAddr);

	// Create the events
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, signalName->c_str());
	delete signalName;

	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE)
	{
		*success = false;
		return;
	}

	std::string* signalAvailName = getMappedName(SIGNAL_AVALIABLE_NAME,
			connectAddr);
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, signalAvailName->c_str());
	delete signalAvailName;
	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE)
	{
		*success = false;
		return;
	}
#elif __APPLE__ || __linux__

	std::string* signalName = getMappedName(SIGNAL_NAME, connectAddr);
	m_hSignal = sem_open(signalName->c_str(), 0);

	if (m_hSignal == (sem_t *) SEM_FAILED)
	{
		printf("client_create: failed: sem_open signal: %s\n", strerror( errno));
		*success = false;
	}
	delete signalName;

	std::string* signalAvailName = getMappedName(SIGNAL_AVALIABLE_NAME, connectAddr);
	m_hAvail = sem_open(signalAvailName->c_str(), 0);
	if (m_hAvail == (sem_t *) SEM_FAILED)
	{
		printf("client_create: failed: sem_open signal: %s\n", strerror( errno));
		*success = false;
	}
	delete signalAvailName;

#endif

	*success = true;
	DEBUG_PRINTF(("IPC Client connected to shared memory %s, status %d\n",this->m_name->c_str(),*success));
}

FmiIpc::Client::~Client()
{
	// Close the event
	if (m_hSignal)
	{
		FmiIpc::close(m_hSignal);
	}

	// Close the event
	if (m_hAvail)
	{
		FmiIpc::close(m_hAvail);
	}

	// Unmap the memory
	if (m_pBuf)
	{
		FmiIpc::unmap(m_pBuf, m_name);
	}

	// Close the file handle
	if (m_hMapFile)
	{
#ifdef _WIN32
		FmiIpc::close(m_hMapFile);
#elif __APPLE__ ||  __linux
#endif
	}
	if (m_name != NULL)
		delete m_name;
}

bool FmiIpc::Client::waitAvailable(DWORD dwTimeout)
{
	// Wait on the available event

#ifdef _WIN32
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
	return false;
#elif __APPLE__ || __linux__
	sem_wait(this->m_hAvail);
#endif

	// Success
	return true;
}
;

SharedFmiMessage* FmiIpc::Client::getMessage(DWORD dwTimeout)
{

	if (waitAvailable(dwTimeout))
	{
		DEBUG_PRINTF(("IPC Client ret msg\n"));

		return &this->m_pBuf->message;
	}
	return NULL;
}
void FmiIpc::Client::sendReply(SharedFmiMessage* reply)
{
	DEBUG_PRINTF(("IPC Client write msg\n"));
	this->m_pBuf->message = *reply;

#ifdef _WIN32
	SetEvent(this->m_hSignal);
#elif __APPLE__ || __linux__

	sem_post(this->m_hSignal);

#endif
	DEBUG_PRINTF(("IPC Client signaled\n"));
}
