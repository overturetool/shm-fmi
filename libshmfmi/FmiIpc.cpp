/*
 * FmiIpc.cpp
 *
 *  Created on: 25/09/2015
 *      Author: kel
 */

#include "FmiIpc.h"
#include <windows.h>
#include <stdio.h>
#include <cstdio>
#include <stddef.h>

FmiIpc::FmiIpc() {
	// TODO Auto-generated constructor stub

}

FmiIpc::~FmiIpc() {
	// TODO Auto-generated destructor stub
}

FmiIpc::Server::Server() {
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
	m_sAddr = NULL;

	//szName = TEXT("Local\\MyFileMappingObject");

	// create the server
	create();
}

FmiIpc::Server::~Server() {
	// Free memory
	if (m_sAddr) {
		free(m_sAddr);
		m_sAddr = NULL;
	}

	// Close the server
	close();
}

void FmiIpc::Server::close() {
	// Close the event
	if (m_hSignal) {
		HANDLE handle = m_hSignal;
		m_hSignal = NULL;
		CloseHandle(handle);
	}

	// Close the event
	if (m_hAvail) {
		HANDLE handle = m_hAvail;
		m_hAvail = NULL;
		CloseHandle(handle);
	}

	// Unmap the memory
	if (m_pBuf) {
		SharedFmiMem *pBuff = m_pBuf;
		m_pBuf = NULL;
		UnmapViewOfFile(pBuff);
	}

	// Close the file handle
	if (m_hMapFile) {
		HANDLE handle = m_hMapFile;
		m_hMapFile = NULL;
		CloseHandle(handle);
	}
}
;

void FmiIpc::Server::create() {
	// Determine the name of the memory
	DWORD ProcessID = GetCurrentProcessId();
	DWORD ThreadID = GetCurrentThreadId();
	//DWORD ServerID = ""//osIPC::GetID();
	TCHAR szName[] = TEXT("Local\\MyFileMappingObject2");
	m_sAddr = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sAddr)
		return;
	sprintf(m_sAddr,"IPC_%04u_%04u_%04u", ProcessID, ThreadID,
			szName);

	char *m_sEvtAvail = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sEvtAvail) {
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	sprintf(m_sEvtAvail, "%s_evt_avail", m_sAddr);
	char *m_sEvtFilled = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sEvtFilled) {
		free(m_sEvtAvail);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	sprintf(m_sEvtFilled, "%s_evt_filled", m_sAddr);

	char *m_sMemName = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sMemName) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	sprintf(m_sMemName, "%s_mem", m_sAddr);
	// Create the events
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, "sigSig"); //(LPCSTR) m_sEvtFilled);
	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, "sigAvail"); /// (LPCSTR) m_sEvtAvail);
	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	// Create the file mapping
	m_hMapFile = CreateFileMapping( INVALID_HANDLE_VALUE,
	NULL,
	PAGE_READWRITE, 0, sizeof(SharedFmiMem), szName);
	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	// Map to the file
	m_pBuf = (SharedFmiMem*) MapViewOfFile(m_hMapFile,	// handle to map object
			FILE_MAP_ALL_ACCESS,	// read/write permission
			0, 0, sizeof(SharedFmiMem));
	if (m_pBuf == NULL) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		printf("server_create: failed: %01d\n", __LINE__);
		return;
	}
	// Clear the buffer
	ZeroMemory(m_pBuf, sizeof(SharedFmiMem));
	m_pBuf->message.cmd = fmi2Reset;

	// Release memory
	free(m_sEvtAvail);
	free(m_sEvtFilled);
	free(m_sMemName);
}

SharedFmiMessage* FmiIpc::Server::send(SharedFmiMessage* message,
		DWORD dwTimeout) {

//	memcpy(&this->m_pBuf->message, message,
	//		sizeof(SharedFmiMessage));
	this->m_pBuf->message = *message;
//	this->m_pBuf->message.cmd =	fmi2Reset;
//this->m_pBuf->message.cmd = message->cmd;
	SetEvent(this->m_hAvail);

	if (WaitForSingleObject(this->m_hSignal, dwTimeout) != WAIT_OBJECT_0) {
		return NULL;
	}

	return &this->m_pBuf->message;
}

FmiIpc::Client::Client() {
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
	m_sAddr = NULL;
}

FmiIpc::Client::Client(const char* connectAddr) {
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	// Determine the name of the memory
	m_sAddr = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sAddr)
		return;
	strcpy_s(m_sAddr, IPC_MAX_ADDR, connectAddr);

	char *m_sEvtAvail = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sEvtAvail)
		return;
	sprintf_s(m_sEvtAvail, IPC_MAX_ADDR, "%s_evt_avail", m_sAddr);

	char *m_sEvtFilled = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sEvtFilled) {
		free(m_sEvtAvail);
		return;
	}
	sprintf_s(m_sEvtFilled, IPC_MAX_ADDR, "%s_evt_filled", m_sAddr);

	char *m_sMemName = (char*) malloc(IPC_MAX_ADDR);
	if (!m_sMemName) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		return;
	}
	sprintf_s(m_sMemName, IPC_MAX_ADDR, "%s_mem", m_sAddr);

	// Create the events
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, "sigSig");	// (LPCSTR) m_sEvtFilled);
	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		return;
	}
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, "sigAvail");// (LPCSTR) m_sEvtAvail);
	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		return;
	}

	// Open the shared file
	m_hMapFile = OpenFileMapping( FILE_MAP_ALL_ACCESS,	// read/write access
			FALSE,					// do not inherit the name
			connectAddr);	// name of mapping object

	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		return;
	}

	// Map to the file
	m_pBuf = (SharedFmiMem*) MapViewOfFile(m_hMapFile,	// handle to map object
			FILE_MAP_ALL_ACCESS,	// read/write permission
			0, 0, sizeof(SharedFmiMem));
	if (m_pBuf == NULL) {
		free(m_sEvtAvail);
		free(m_sEvtFilled);
		free(m_sMemName);
		return;
	}

	// Release memory
	free(m_sEvtAvail);
	free(m_sEvtFilled);
	free(m_sMemName);

}

FmiIpc::Client::~Client() {
	// Close the event
	CloseHandle(m_hSignal);

	// Close the event
	CloseHandle(m_hAvail);

	// Unmap the memory
	UnmapViewOfFile(m_pBuf);

	// Close the file handle
	CloseHandle(m_hMapFile);
}

bool FmiIpc::Client::waitAvailable(DWORD dwTimeout) {
	// Wait on the available event
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
		return false;

	// Success
	return true;
}
;

SharedFmiMessage* FmiIpc::Client::getMessage(DWORD dwTimeout) {

	if (waitAvailable(dwTimeout)) {
		return &this->m_pBuf->message;
	}
	return NULL;
}
void FmiIpc::Client::sendReply(SharedFmiMessage* reply) {

	memcpy(&this->m_pBuf->message, reply, sizeof(SharedFmiMessage));

	SetEvent(this->m_hSignal);
}
