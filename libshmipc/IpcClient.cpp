/*
 * IpcClient.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcClient.h"

namespace FmiIpc {

IpcClient::IpcClient(int id, const char* name) : FmiIpc::IpcBase(id, name) {
  m_log_name_id = "client";
}

IpcClient::~IpcClient() {}

SharedFmiMessage* IpcClient::getMessage(DWORD dwTimeout) {
  if (signalWait(this->m_hAvail, dwTimeout)) {
    dprintf("\tIPC %s %d: Received msg. Type: %d, Size: %d\n", m_log_name_id,
            m_id, this->m_pBuf->message.cmd,
            this->m_pBuf->message.protoBufMsgSize);

    return &this->m_pBuf->message;
  }
  return NULL;
}
void IpcClient::sendReply(SharedFmiMessage* reply) {
  dprintf("\tIPC %s %d: Write msg. Type: %d, Size: %d\n", m_log_name_id, m_id,
          reply->cmd, reply->protoBufMsgSize);
  this->m_pBuf->message = *reply;

  signalPost(this->m_hSignal);
  dprintf("\tIPC %s %d: Signaled\n", m_log_name_id, m_id);
}

void IpcClient::connect(bool* success) {
  // Set default params
  *success = true;
  m_hMapFile = 0;
  m_hSignal = 0;
  m_hAvail = 0;
  m_pBuf = NULL;

  //	enableConsoleDebug();

  std::string nameOfMapping =
      getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
  //  dprintf("\tIPC %s: Starting with key: %s\n",m_log_name_id,
  //  nameOfMapping.c_str());
  dprintf("\tIPC %s %d: Connecting with key %s\n", m_log_name_id, m_id,
          nameOfMapping.c_str());
  // Open the shared file

  m_hMapFile = openShm(success, nameOfMapping.c_str());
  m_hMapFileName = strdup(nameOfMapping.c_str());
  if (!*success) {
    dprintf("\tIPC %s %d: Failed to connect to shared memory %s, status %d\n",
            m_log_name_id, m_id, this->m_name->c_str(), *success);
    return;
  }

  // Map to the file

  mapShm(success, m_hMapFile, false);
  if (!*success) {
    dprintf("\tIPC %s %d: Failed to map shared memory %s, status %d\n",
            m_log_name_id, m_id, this->m_name->c_str(), *success);
    return;
  }

  m_hSignal = this->createSignal(success, SIGNAL_NAME);
  if (!*success) {
    dprintf("\tIPC %s %d: Failed to create 'signal' signal\n", m_log_name_id,
            m_id, this->m_name->c_str(), *success);
    return;
  }
  m_hAvail = this->createSignal(success, SIGNAL_AVALIABLE_NAME);
  if (!*success) {
    dprintf("\tIPC %s %d: Failed to create 'available' signal\n", m_log_name_id,
            m_id, this->m_name->c_str(), *success);
    return;
  }

  dprintf("\tIPC %s %d: Connected to shared memory %s, status %d\n",
          m_log_name_id, m_id, this->m_name->c_str(), *success);
}

} /* namespace FmiIpc */
