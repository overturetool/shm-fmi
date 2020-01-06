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

#ifndef IPCCLIENT_H_
#define IPCCLIENT_H_

#include "IpcBase.h"

namespace FmiIpc {

class IpcClient : public IpcBase {
 public:
  IpcClient(int id, const char* name = "server");
  virtual ~IpcClient();

  void connect(bool* success);

 public:
  SharedFmiMessage* getMessage(DWORD dwTimeout = INFINITE);
  void sendReply(SharedFmiMessage* reply);
  void waitForWatchDogEvent(DWORD dwTimeout = INFINITE);

  // Functions
  BOOL IsOk(void) {
    if (m_pBuf)
      return true;
    else
      return false;
  };
};

} /* namespace FmiIpc */

#endif /* IPCCLIENT_H_ */
