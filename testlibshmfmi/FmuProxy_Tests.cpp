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
#include "gtest/gtest.h"

#include "FmuProxy.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"

#include "IpcClient.h"

#include "FmuProxyTest.h"

/*TEST_F(FmuProxyTest, system)
 {
 std::eexec("java -cp
 .:/home/parallels/Downloads/grpc-fmi/java/bin/:/home/parallels/Downloads/grpc-fmi/java/lib/netty-all-4.1.0.Beta5.jar:/home/parallels/Downloads/grpc-fmi/java/lib/grpc-all-0.8.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/guava-18.0.jar:/home/parallels/Downloads/grpc-fmi/java/lib/hpack-0.10.1.jar:/home/parallels/Downloads/grpc-fmi/java/lib/protobuf-java-3.0.0-alpha-3.1.jar
 rpc.Server");
 }*/

void clientThread() {
  bool success;
  FmiIpc::IpcClient client(0, MEM_KEY);
  client.connect(&success);
  if (!success) {
    // recurse until connected
    clientThread();
    return;
  }
  SharedFmiMessage* msg = client.getMessage(0);
  printf("Client got message\n");
  msg->cmd = fmi2DoStep;

  printf("Client is sending back reply now\n");
  client.sendReply(msg);
  printf("Client terminating\n");
  return;
}

TEST(FmiIpc, shmbasetest) {
  FmiIpc::IpcServer server(0, MEM_KEY);

  EXPECT_EQ(true, server.create());

  std::thread t1(clientThread);

  printf("Server is sending message to client\n");

  SharedFmiMessage msg;

  msg.cmd = fmi2SetupExperiment;

  server.send(&msg, 0);

  printf("Server got reply from client\n");

  t1.join();
  EXPECT_EQ(true, true);
  server.close();
}

void remoteClientThread() { remoteTestDriverSingle(MEM_KEY); }

void remoteClientThreadResume() { remoteTestDriver(MEM_KEY); }

TEST_F(FmuProxyTest, fmi2Instantiate) {
  std::string instanceName = "my instance";
  std::string fmuGUID = "dcnkjvnrevirehvljkfnvf";
  std::string fmuResourceLocation = "/tmp";
  const char* callbackAddress = "localhost";
  int callbackPort = 8080;
  bool visible = true;
  bool loggingOn = true;

  EXPECT_EQ(true,
            m_client->fmi2Instantiate(instanceName.c_str(), fmuGUID.c_str(),
                                      fmuResourceLocation.c_str(),
                                      callbackAddress, visible, loggingOn));
}

TEST_F(FmuProxyTest, fmi2FreeInstantiate) { m_client->fmi2FreeInstance(); }

TEST_F(FmuProxyTest, fmi2EnterInitializationMode) {
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2EnterInitializationMode());
}

TEST_F(FmuProxyTest, fmi2ExitInitializationMode) {
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2ExitInitializationMode());
}

TEST_F(FmuProxyTest, fmi2SetupExperiment) {
  bool toleranceDefined = true;
  double tolerance = 0.1;
  double startTime = 0.0;
  bool stopTimeDefined = true;
  double stopTime = 1.1;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2SetupExperiment(
                                  toleranceDefined, tolerance, startTime,
                                  stopTimeDefined, stopTime));
}

TEST_F(FmuProxyTest, fmi2Terminate) {
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2Terminate());
}

TEST_F(FmuProxyTest, fmi2SetDebugLogging) {
  int loggingOn = true;
  size_t nCategories = 3;
  const char* categories[3] = {"all", "all+1", "all+2"};

  EXPECT_EQ(FmuProxy::fmi2OK,
            m_client->fmi2SetDebugLogging(loggingOn, nCategories, categories));
}

TEST_F(FmuProxyTest, SetGetReals) {
  unsigned int vr[] = {1, 2, 3};
  double vals[] = {1.1, 2.2, 3.3};

  int size = 3;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2SetReal(vr, size, vals));

  this->resetServiceThread();

  double res[size];

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetReal(vr, size, res));

  for (int i = 0; i < size; i++) {
    EXPECT_EQ(vals[i], res[i]);
  }
}

TEST_F(FmuProxyTest, SetGetBools) {
  unsigned int vr[] = {1, 2, 3};
  int vals[] = {true, false, true};

  int size = 3;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2SetBoolean(vr, size, vals));

  this->resetServiceThread();

  int res[size];
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetBoolean(vr, size, res));

  for (int i = 0; i < size; i++) {
    EXPECT_EQ(vals[i], res[i]);
  }
}

TEST_F(FmuProxyTest, SetGetIntegers) {
  unsigned int vr[] = {1, 2, 3};
  int vals[] = {true, false, true};

  int size = 3;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2SetInteger(vr, size, vals));

  this->resetServiceThread();

  int res[size];
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetInteger(vr, size, res));

  for (int i = 0; i < size; i++) {
    EXPECT_EQ(vals[i], res[i]);
  }
}

TEST_F(FmuProxyTest, SetGetStrings) {
  unsigned int vr[] = {1, 2, 3};

  std::string vals[] = {"Test string: a", "Test string: b", "Test string: c"};

  int size = 3;

  const char* svals[size];

  for (int i = 0; i < size; i++) {
    svals[i] = vals[i].c_str();
  }

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2SetString(vr, size, svals));

  this->resetServiceThread();

  const char* res[size];
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetString(vr, size, res));

  for (int i = 0; i < size; i++) {
    ASSERT_STREQ(svals[i], res[i]);
  }
}

TEST_F(FmuProxyTest, SetGetStringsOverflow) {
  unsigned int vr[] = {1, 2, 3};


  int size = 1;

  const char* svals[size];

   char s[SharedFmiBufferSize+1];
    svals[0] = s;

    std::size_t length = std::string(SharedFmiBufferSize,'a').copy(s,SharedFmiBufferSize);
      s[length]='\0';

  EXPECT_EQ(FmuProxy::fmi2Fatal, m_client->fmi2SetString(vr, size, svals));


  const char* res[size];
  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetString(vr, size, res));


}

TEST_F(FmuProxyTest, fmi2DoStep) {
  double currentCommunicationPoint = 0.0;
  double communicationStepSize = 0.1;
  int noSetFMUStatePriorToCurrentPoint = false;

  EXPECT_EQ(
      FmuProxy::fmi2OK,
      m_client->fmi2DoStep(currentCommunicationPoint, communicationStepSize,
                           noSetFMUStatePriorToCurrentPoint));
}

TEST_F(FmuProxyTest, fmi2GetStatus) {
  FmuProxy::fmi2StatusKind s = FmuProxy::fmi2LastSuccessfulTime;
  FmuProxy::fmi2Status value;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetStatus(s, &value));
  EXPECT_EQ(FmuProxy::fmi2OK, value);
}

TEST_F(FmuProxyTest, fmi2GetRealStatus) {
  FmuProxy::fmi2StatusKind s = FmuProxy::fmi2LastSuccessfulTime;
  double value;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetRealStatus(s, &value));
  EXPECT_EQ(100.5, value);
}

TEST_F(FmuProxyTest, fmi2GetIntegerStatus) {
  FmuProxy::fmi2StatusKind s = FmuProxy::fmi2LastSuccessfulTime;
  int value;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetIntegerStatus(s, &value));
  EXPECT_EQ(100, value);
}

TEST_F(FmuProxyTest, fmi2GetBooleanStatus) {
  FmuProxy::fmi2StatusKind s = FmuProxy::fmi2Terminated;
  int value;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetBooleanStatus(s, &value));
  EXPECT_EQ(true, value);
}

TEST_F(FmuProxyTest, fmi2GetStringStatus) {
  FmuProxy::fmi2StatusKind s = FmuProxy::fmi2DoStepStatus;
  const char* value;

  EXPECT_EQ(FmuProxy::fmi2OK, m_client->fmi2GetStringStatus(s, &value));
  ASSERT_STREQ("waiting", value);
}
