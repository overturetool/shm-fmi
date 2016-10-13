/*
 * IpcClient.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcClient.h"

namespace FmiIpc
{

IpcClient::IpcClient(int id, bool* success, const char* name) :
		FmiIpc::IpcBase(id, name)
{
	this->connect(success);
}

IpcClient::~IpcClient()
{
}

SharedFmiMessage* IpcClient::getMessage(DWORD dwTimeout)
{
	if (signalWait(this->m_hAvail,dwTimeout))
	{
		dprint("IPC Client received msg\n");

		return &this->m_pBuf->message;
	}
	return NULL;
}
void IpcClient::sendReply(SharedFmiMessage* reply)
{
	dprint("IPC Client write msg\n");
	this->m_pBuf->message = *reply;

	signalPost(this->m_hSignal);
	dprint("IPC Client signaled\n");
}

void IpcClient::connect(bool* success)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	enableConsoleDebug();

	dprintf("IPC Client key %s\n", this->m_name->c_str());

	std::string nameOfMapping = getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
	dprintf("Starting IPC client with key: %s\n", nameOfMapping.c_str());
	// Open the shared file

	m_hMapFile = openShm(success, nameOfMapping.c_str());
	m_hMapFileName = strdup(nameOfMapping.c_str());

	// Map to the file

	mapShm(success, m_hMapFile, false);

	m_hSignal = this->createSignal(SIGNAL_NAME);
	m_hAvail = this->createSignal(SIGNAL_AVALIABLE_NAME);
	*success = m_hSignal != NULL && m_hAvail != NULL;

	*success = true;
	dprintf("IPC Client connected to shared memory %s, status %d\n", this->m_name->c_str(), *success);
}

} /* namespace FmiIpc */
