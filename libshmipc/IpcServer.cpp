/*
 * IpcServer.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcServer.h"

namespace FmiIpc
{

IpcServer::IpcServer(int id, const char* name) :
		FmiIpc::IpcBase(id, name)
{

}

IpcServer::~IpcServer()
{
	this->close();
}

bool IpcServer::create()
{
	bool ok = true;

//	enableConsoleDebug();

	std::string nameOfMapping = getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
	dprintf("Starting IPC server with key: %s\n", nameOfMapping.c_str());
	// Create the file mapping

	m_hMapFile = openShm(&ok, nameOfMapping.c_str(), true);
	if (!ok)
	{
		dprintf("Error in IPC server could not create shared memory key: %s\n", nameOfMapping.c_str());
		return false;
	}
	m_hMapFileName = strdup(nameOfMapping.c_str());
	mapShm(&ok, m_hMapFile, true);
	if (!ok)
	{
		dprintf("Error in IPC server could not map shared memory key: %s\n", nameOfMapping.c_str());
		return false;
	}

	// Create the events
	this->m_hSignal = this->createSignal(SIGNAL_NAME, true);
	this->m_hAvail = this->createSignal(SIGNAL_AVALIABLE_NAME, true);
	ok = this->m_hSignal != NULL && this->m_hAvail != NULL;

	if (this->m_pBuf == NULL || this->m_hSignal == NULL || this->m_hAvail == NULL)
	{
		if (this->m_pBuf == NULL)
		{
			dprintf("Error in IPC server create key: %s, buf=NULL\n", nameOfMapping.c_str());
		}
		if (this->m_hSignal == NULL)
		{
			dprintf("Error in IPC server create key: %s, signal=NULL\n", nameOfMapping.c_str());
		}
		if (this->m_hSignal == NULL)
		{
			dprintf("Error in IPC server create key: %s, avail=NULL\n", nameOfMapping.c_str());
		}
		ok = false;
	}

	return ok;
}
void IpcServer::close(void)
{

}
SharedFmiMessage* IpcServer::send(SharedFmiMessage* message, DWORD dwTimeout)
{
	dprint("IPC Server write msg\n");
	this->m_pBuf->message = *message;

	signalPost(this->m_hAvail);

	if (!signalWait(this->m_hSignal, dwTimeout))
	{
		return NULL;
	}

	dprint("IPC Server received data\n");
	return &this->m_pBuf->message;
}

} /* namespace FmiIpc */
