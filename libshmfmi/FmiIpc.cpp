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

const char* FmiIpc::SHARED_MEM_BASE_NAME = "Local\\";
const char* FmiIpc::SIGNAL_AVALIABLE_NAME = "sigAvail";
const char* FmiIpc::SIGNAL_NAME = "sig";

FmiIpc::FmiIpc()
{
	// TODO Auto-generated constructor stub

}

FmiIpc::~FmiIpc()
{
	// TODO Auto-generated destructor stub
}

std::string* getMappedName(const char* baseName, const char* name)
{
	std::string* nameOfMapping = new std::string(baseName);
	*nameOfMapping += std::string(name);
	return nameOfMapping;
}

FmiIpc::Server::Server(const char* connectionName)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	// create the server
	create(connectionName);
}

FmiIpc::Server::~Server()
{
	// Free memory

	// Close the server
	close();

	if (m_name != NULL)
		delete m_name;
}

void FmiIpc::Server::close()
{
	// Close the event
	if (m_hSignal)
	{
		HANDLE handle = m_hSignal;
		m_hSignal = NULL;
		FmiIpc::close(handle);
	}

	// Close the event
	if (m_hAvail)
	{
		HANDLE handle = m_hAvail;
		m_hAvail = NULL;
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
		m_hMapFile = NULL;

#ifdef _WIN32
		FmiIpc::close(m_hMapFile);
#elif __APPLE__ ||  __linux
//POSIX
	shm_unlink(m_name->c_str());
#endif

	}
}
;

void FmiIpc::Server::create(const char* name)
{

	std::string* nameOfMapping = getMappedName(SHARED_MEM_BASE_NAME, name);
	printf("Starting server with key: %s\n", nameOfMapping->c_str());
	// Create the file mapping

#ifdef _WIN32
	m_hMapFile = CreateFileMapping( INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE, 0, sizeof(SharedFmiMem), nameOfMapping->c_str());
#elif __APPLE__ ||  __linux
//POSIX
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

		printf("server_create: failed: %01d\n", __LINE__);
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
	// Clear the buffer
	ZeroMemory(m_pBuf, sizeof(SharedFmiMem));
#elif __APPLE__ ||  __linux
//POSIX

	if (ftruncate(m_hMapFile, sizeof(SharedFmiMem)) != 0)
	{
		printf("server_create: failed: unable to truncate\n");
		return;
	}

	void *ptr = mmap(0, sizeof(SharedFmiMem), PROT_READ | PROT_WRITE,
	MAP_SHARED, m_hMapFile, 0);
	if (ptr == MAP_FAILED)
	{
		printf("server_create: failed: mmap\n");
		return;
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
		return;
	}

	std::string* signalAvailName = getMappedName(SIGNAL_AVALIABLE_NAME, name);
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, signalAvailName->c_str());
	delete signalAvailName;

	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE)
	{
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
#elif __APPLE__ ||  __linux
//POSIX
	sem_init(&m_pBuf->semSignal, 1, 0);
	sem_init(&m_pBuf->semAvail, 1, 0);
#endif

	m_pBuf->message.cmd = fmi2Reset;
}

SharedFmiMessage* FmiIpc::Server::send(SharedFmiMessage* message,
		DWORD dwTimeout)
{

	this->m_pBuf->message = *message;


#ifdef _WIN32
	SetEvent(this->m_hAvail);

		if (WaitForSingleObject(this->m_hSignal, dwTimeout) != WAIT_OBJECT_0)
		{
			return NULL;
		}
#elif __APPLE__ ||  __linux
//POSIX
	sem_post(&this->m_pBuf->semAvail);
	sem_wait(&this->m_pBuf->semSignal);
#endif



	return &this->m_pBuf->message;
}

FmiIpc::Client::Client()
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
}

FmiIpc::Client::Client(const char* connectAddr, bool* success)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	printf("Client key %s\n", connectAddr);



	std::string* nameOfMapping = getMappedName(SHARED_MEM_BASE_NAME,
			connectAddr);
	printf("Starting client with key: %s\n", nameOfMapping->c_str());
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

		printf("Starting client with key: %s - faild\n",
				nameOfMapping->c_str());
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
#elif __APPLE__ ||  __linux
//POSIX

#endif





	*success = true;
}

FmiIpc::Client::~Client()
{
	// Close the event
	if(m_hSignal)
	FmiIpc::close(m_hSignal);


	// Close the event
	if(m_hAvail)
	FmiIpc::close(m_hAvail);

	// Unmap the memory
	//UnmapViewOfFile(m_pBuf);
	if(m_pBuf)
	FmiIpc::unmap(m_pBuf, m_name);

	// Close the file handle
	if(m_hMapFile)
#ifdef _WIN32
		FmiIpc::close(m_hMapFile);
#elif __APPLE__ ||  __linux
//POSIX
	shm_unlink(m_name->c_str());
#endif


	if (m_name != NULL)
		delete m_name;
}

bool FmiIpc::Client::waitAvailable(DWORD dwTimeout)
{
	// Wait on the available event


#ifdef _WIN32
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
			return false;
#elif __APPLE__ ||  __linux
//POSIX
	sem_wait(&this->m_pBuf->semAvail);
#endif


	// Success
	return true;
}
;

SharedFmiMessage* FmiIpc::Client::getMessage(DWORD dwTimeout)
{

	if (waitAvailable(dwTimeout))
	{
		return &this->m_pBuf->message;
	}
	return NULL;
}
void FmiIpc::Client::sendReply(SharedFmiMessage* reply)
{

	memcpy(&this->m_pBuf->message, reply, sizeof(SharedFmiMessage));



#ifdef _WIN32
	SetEvent(this->m_hSignal);
#elif __APPLE__ ||  __linux
//POSIX
	sem_post(&this->m_pBuf->semSignal);
#endif
}
