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
	// TODO Auto-generated constructor stub

}

IpcServer::~IpcServer()
{
	this->close();

}

bool IpcServer::create()
{
	bool ok = true;
	std::string* nameOfMapping = getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
	dprintf("Starting IPC server with key: %s\n", nameOfMapping->c_str());
	// Create the file mapping

	m_hMapFile = openShm(&ok, nameOfMapping->c_str(), true);
	mapShm(&ok, m_hMapFile, true);

	// Create the events
	this->m_hSignal = this->createSignal(SIGNAL_NAME, true);
	this->m_hAvail = this->createSignal(SIGNAL_AVALIABLE_NAME, true);
	ok = this->m_hSignal != NULL && this->m_hAvail != NULL;

	if (m_pBuf == NULL)
	{
		dprint("Error in IPC server ctr, buf NULL\n");
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

#ifdef _WIN32
	SetEvent(this->m_hAvail);
	dprint( "IPC Server signaled\n");

	if (WaitForSingleObject(this->m_hSignal, dwTimeout) != WAIT_OBJECT_0)
	{
		return NULL;
	}
#elif __APPLE__ || __linux

	if (sem_post(this->m_hAvail) == -1)
	{
		dprintf("signal: failed: sem_postn signal 'm_hAvail': %s\n", strerror( errno));
	}

	dprint("IPC Server signaled data ready. Waiting...\n");

	if (sem_wait(this->m_hSignal) == -1)
	{
		dprintf("signal: failed: sem_wait signal 'm_hSignal': %s\n", strerror( errno));
	}

#endif

	dprint("IPC Server received data\n");
	return &this->m_pBuf->message;
}

} /* namespace FmiIpc */
