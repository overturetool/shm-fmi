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

#include "IpcServer.h"
#include "IpcClient.h"

using namespace sharedfmimemory;

int main(int argc, char* argv[]) {
  const char* key = argv[1];

  printf("Key is %s\n", key);

  bool serverMode = strcmp("client", argv[2]);

  if (serverMode) {
    printf("Server mode selected\n");

    FmiIpc::IpcServer* server = new FmiIpc::IpcServer(0, key);
    server->create();
    SharedFmiMessage message;

    message.cmd = fmi2Command::fmi2DoStep;

    const char* data = argv[3];
    int length = strlen(data);

    for (int i = 0; i < length; i++) {
      message.protoBufMsg[i] = data[i];
    }

    message.protoBufMsgSize = length;

    printf("Sending %s, with length %i\n", message.protoBufMsg,
           message.protoBufMsgSize);
    server->send(&message, 0);

    server->close();
    delete server;
  } else {
    printf("Client mode selected\n");

    bool success;

    FmiIpc::IpcClient* client = NULL;

    while (!success) {
      printf("Trying to connect and answer. Key: %s\n", key);
      client = new FmiIpc::IpcClient(1, key);
      client->connect(&success);
    }

    printf("Connected.\n");

    printf("Waiting to receive message\n");
    SharedFmiMessage* message = client->getMessage(0);

    if (message == NULL) {
      printf("Did only receive a NULL\n");
      delete client;
      return 0;
    }

    printf("Received Cmd: %i, Data: %s, Length: %i\n", message->cmd,
           message->protoBufMsg, message->protoBufMsgSize);
    client->sendReply(message);
    delete client;
    return 0;
  }
}
