/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */

#ifndef IPCSERVER_H_
#define IPCSERVER_H_

#include "IpcBase.h"
#include <thread>

#ifdef __MINGW32__
// Mingw 5.3.1 > has a different implementation of <thread> than the Mingw
// 4.8.2 has so this fixes this
#include "mingw.thread.h"
#endif

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
