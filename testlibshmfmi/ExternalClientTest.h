/*
 * ExternalClientTest.h
 *
 *  Created on: Apr 26, 2016
 *      Author: kel
 */

#ifndef EXTERNALCLIENTTEST_H_
#define EXTERNALCLIENTTEST_H_

#include "gtest/gtest.h"

#include "ExternalClient.h"
#include <string>
#include <iostream>
#include <thread>
#include "RemoteTestDriver.h"

#define MEM_KEY "shmFmiTest"

void remoteClientThread();
void remoteClientThreadResume();

class ExternalClientTest: public ::testing::Test
{
public:
	ExternalClientTest()
	{
		m_client = NULL;
	}

	void SetUp()
	{
		m_client = new ExternalClient(MEM_KEY);

		EXPECT_EQ(true, m_client->initialize());

		t1 = new std::thread(remoteClientThread);
	}

	void TearDown()
	{
		if (m_client != NULL)
		{
			delete m_client;
		}
		t1->join();
	}

	~ExternalClientTest()
	{
		// cleanup any pending stuff, but no exceptions allowed
	}

	void resetServiceThread()
	{
		t1->join();
		t1 = new std::thread(remoteClientThreadResume);
	}

	ExternalClient* m_client;

	std::thread* t1;

private:

public:

};

#endif /* EXTERNALCLIENTTEST_H_ */
