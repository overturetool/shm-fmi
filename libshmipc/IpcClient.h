/*
 * IpcClient.h
 *
 *  Created on: Aug 23, 2016
 *      Author: kel
 */

#ifndef IPCCLIENT_H_
#define IPCCLIENT_H_

#include "IpcBase.h"

namespace FmiIpc {

class IpcClient : public IpcBase {
 public:
  IpcClient(int id, bool* success, const char* name = "server");
  virtual ~IpcClient();

 private:
  void connect(bool* success);

 public:
  SharedFmiMessage* getMessage(DWORD dwTimeout = INFINITE);
  void sendReply(SharedFmiMessage* reply);

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
