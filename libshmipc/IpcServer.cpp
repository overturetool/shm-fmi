/*
 * IpcServer.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#include "IpcServer.h"

namespace FmiIpc {

const int IpcServer::ALIVE_DELAY = 200;

IpcServer::IpcServer(int id, const char* name) : FmiIpc::IpcBase(id, name) {
  m_log_name_id = "server";
  m_connected = false;
  m_connWatchDogThread = NULL;
}

IpcServer::~IpcServer() {
  this->close();
  this->m_connected = false;
  this->m_connWatchDogThread->join();
  delete m_connWatchDogThread;
}

void IpcServer::connWatchDog() {
  while (m_connected) {
    signalPost(m_hConnWatchDogSignal);
    this->sleep(ALIVE_DELAY);
  }
}

bool IpcServer::create() {
  bool success = true;

  //	enableConsoleDebug();

  std::string nameOfMapping =
      getMappedName(this, SHARED_MEM_BASE_NAME, this->m_name->c_str());
  dprintf("\tIPC %s %d: Starting with key: %s\n", m_log_name_id, m_id,
          nameOfMapping.c_str());
  // Create the file mapping

  m_hMapFile = openShm(&success, nameOfMapping.c_str(), true);
  if (!success) {
    dprintf("\tIPC %s %d: Error could not create shared memory key: %s\n",
            m_log_name_id, m_id, nameOfMapping.c_str());
    return false;
  }
  m_hMapFileName = strdup(nameOfMapping.c_str());
  mapShm(&success, m_hMapFile, true);
  if (!success) {
    dprintf("\tIPC %s %d: Error could not map shared memory key: %s\n",
            m_log_name_id, m_id, nameOfMapping.c_str());
    return false;
  }

  // Create the events
  this->m_hSignal = this->createSignal(&success, SIGNAL_NAME, true);
  if (!success) {
    dprintf("\tIPC %s %d: Error create key: %s, signal=NULL\n", m_log_name_id,
            m_id, nameOfMapping.c_str());
    return success;
  }

  this->m_hAvail = this->createSignal(&success, SIGNAL_AVALIABLE_NAME, true);
  if (!success) {
    dprintf("\tIPC %s %d: Error create key: %s, avail=NULL\n", m_log_name_id,
            m_id, nameOfMapping.c_str());

    return success;
  }

  this->m_hConnWatchDogSignal =
      this->createSignal(&success, SIGNAL_CONN_WATCH_DOG_NAME, true);
  if (!success) {
    dprintf("\tIPC %s %d: Error create key: %s, conn watch dog=NULL\n",
            m_log_name_id, m_id, nameOfMapping.c_str());

    return success;
  }

  this->m_connected = true;
  this->m_connWatchDogThread = new std::thread(&IpcServer::connWatchDog, this);

  return success;
}
void IpcServer::close(void) { this->m_connected = false; }

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
