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

#ifndef EXTERNALCLIENTTEST_H_
#define EXTERNALCLIENTTEST_H_

#include "gtest/gtest.h"

#include "FmuProxy.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"

#ifdef _WIN32
#define MEM_KEY "shmFmiTestWin"
#elif __APPLE__
    // POSIX
#define MEM_KEY "shmFmiTestApple"
#elif __linux
#define MEM_KEY "shmFmiTestLinux"
#endif



void remoteClientThread();
void remoteClientThreadResume();

class FmuProxyTest : public ::testing::Test {
 public:
  FmuProxyTest() {
    m_client = NULL;
    t1 = NULL;
  }

  void SetUp() {
    m_client = new FmuProxy(0, MEM_KEY);
    m_client->getChannel()->enableConsoleDebug();

    EXPECT_EQ(true, m_client->initialize());

    t1 = new std::thread(remoteClientThread);
  }

  void TearDown() {
    if (m_client != NULL) {
      delete m_client;
    }
    t1->join();
  }

  ~FmuProxyTest() {
    // cleanup any pending stuff, but no exceptions allowed
  }

  void resetServiceThread() {
    t1->join();
    t1 = new std::thread(remoteClientThreadResume);
  }

  FmuProxy* m_client;

  std::thread* t1;

 private:
 public:
};

#endif /* EXTERNALCLIENTTEST_H_ */
