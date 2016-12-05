/*
 * IpcServer.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcServer.h"

namespace FmiIpc {

IpcServer::IpcServer(int id, const char* name) : FmiIpc::IpcBase(id, name) {
  m_log_name_id = "server";
}

IpcServer::~IpcServer() { this->close(); }

bool IpcServer::create() {
  bool ok = true;

  //	enableConsoleDebug();

  std::string nameOfMapping =
      getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
  dprintf("\tIPC %s %d: Starting with key: %s\n", m_log_name_id, m_id,
          nameOfMapping.c_str());
  // Create the file mapping

  m_hMapFile = openShm(&ok, nameOfMapping.c_str(), true);
  if (!ok) {
    dprintf("\tIPC %s %d: Error could not create shared memory key: %s\n",
            m_log_name_id, m_id, nameOfMapping.c_str());
    return false;
  }
  m_hMapFileName = strdup(nameOfMapping.c_str());
  mapShm(&ok, m_hMapFile, true);
  if (!ok) {
    dprintf("\tIPC %s %d: Error could not map shared memory key: %s\n",
            m_log_name_id, m_id, nameOfMapping.c_str());
    return false;
  }

  // Create the events
  this->m_hSignal = this->createSignal(&ok, SIGNAL_NAME, true);
  if (!ok) {
    dprintf("\tIPC %s %d: Error create key: %s, signal=NULL\n", m_log_name_id,
            m_id, nameOfMapping.c_str());
    return ok;
  }

  this->m_hAvail = this->createSignal(&ok, SIGNAL_AVALIABLE_NAME, true);
  if (!ok) {
    dprintf("\tIPC %s %d: Error create key: %s, avail=NULL\n", m_log_name_id,
            m_id, nameOfMapping.c_str());
    return ok;
  }

  return ok;
}
void IpcServer::close(void) {}
SharedFmiMessage* IpcServer::send(SharedFmiMessage* message, DWORD dwTimeout) {
  dprintf("\tIPC %s %d: Writing msg. Type: %d, Size: %d\n", m_log_name_id, m_id,
          message->cmd, message->protoBufMsgSize);
  this->m_pBuf->message = *message;

  signalPost(this->m_hAvail);

  if (!signalWait(this->m_hSignal, dwTimeout)) {
    return NULL;
  }

  dprintf("\tIPC %s %d: Received msg. Type: %d, Size: %d\n", m_log_name_id,
          m_id, this->m_pBuf->message.cmd,
          this->m_pBuf->message.protoBufMsgSize);
  return &this->m_pBuf->message;
}

} /* namespace FmiIpc */
