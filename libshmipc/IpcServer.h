/*
 * IpcServer.h
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#ifndef IPCSERVER_H_
#define IPCSERVER_H_

#include "IpcBase.h"
#include <thread>

namespace FmiIpc {

class IpcServer : public IpcBase {
 public:
  IpcServer(int id, const char* name = "server");
  virtual ~IpcServer();

 public:
  // Create and destroy functions
  bool create();
  void close(void);
  SharedFmiMessage* send(SharedFmiMessage* message, DWORD dwTimeout);

 private:
  void connWatchDog();
  bool m_connected;
  std::thread* m_connWatchDogThread;
  static const int ALIVE_DELAY;
};

} /* namespace FmiIpc */

#endif /* IPCSERVER_H_ */
