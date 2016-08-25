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

bool IpcClient::waitAvailable(DWORD dwTimeout)
{
	// Wait on the available event

#ifdef _WIN32
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
	return false;
#elif __APPLE__ || __linux__

	if (dwTimeout == 0)
	{
		if (sem_wait(this->m_hAvail) == -1)
		{
			return false;
		}
	} else
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);

		ts.tv_sec = ts.tv_sec + (dwTimeout / 1000);

		ts.tv_sec = ts.tv_sec + (dwTimeout / 1000);
		ts.tv_nsec = ts.tv_nsec + ((dwTimeout % 1000) * 1000000);

		if (sem_timedwait(this->m_hAvail, &ts) == -1)
		{
			return false;
		}
	}
#endif

	// Success
	return true;
}
;

SharedFmiMessage* IpcClient::getMessage(DWORD dwTimeout)
{
	if (waitAvailable(dwTimeout))
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

#ifdef _WIN32
	SetEvent(this->m_hSignal);
#elif __APPLE__ || __linux__

	sem_post(this->m_hSignal);

#endif
	dprint("IPC Client signaled\n");
}

void IpcClient::connect(bool* success)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

//	enableConsoleDebug();

	dprintf("IPC Client key %s\n", this->m_name->c_str());

	std::string* nameOfMapping = getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
	dprintf("Starting IPC client with key: %s\n", nameOfMapping->c_str());
	// Open the shared file

	m_hMapFile = openShm(success, nameOfMapping->c_str());

	// Map to the file

	mapShm(success, m_hMapFile, false);

	m_hSignal = this->createSignal(SIGNAL_NAME);
	m_hAvail = this->createSignal(SIGNAL_AVALIABLE_NAME);
	*success = m_hSignal != NULL && m_hAvail != NULL;

	*success = true;
	dprintf("IPC Client connected to shared memory %s, status %d\n", this->m_name->c_str(), *success);
}

} /* namespace FmiIpc */
