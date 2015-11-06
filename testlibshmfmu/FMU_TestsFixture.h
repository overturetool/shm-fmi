/*
 * FMU_TestsFixture.h
 *
 *  Created on: Oct 6, 2015
 *      Author: parallels
 */

#ifndef FMU_H_
#define FMU_H_

#include "gtest/gtest.h"

//file io
#include <iostream>
#include <fstream>

//current dir
#include <unistd.h>

class FMUTest: public ::testing::Test
{
public:
	FMUTest()
	{
		// initialization code here
//		pid = 0;
	}

	void SetUp()
	{
		// code here will execute just before the test ensues
		//   printf("Ready for forking\n");
//	   pid = fork();
//	   	if (pid == 0)
//	   	{
//	   		std::this_thread::sleep_for(std::chrono::milliseconds
//	   		(1000));
//	   		printf("test driver forked\n");
//	   		execl("./testlibshmfmu-driver","./testlibshmfmu-driver", (char*) 0);
//	   	 fputs(dlerror(),stderr);
//	   	//        fprintf(stderr,dlerror());
//	   	        fflush(stderr);
//	   		exit(0);
//	   	} else if(pid==-1)
//	   	{
//	   		// Parent process will return a non-zero value from fork()
//	   		printf("server_create: failed to fork: %01d %s\n", __LINE__,strerror( errno ));
//
//
//	   	}else{
//
//	   	}

		fmu = setup();
	}

	void TearDown()
	{
		// code here will be called just after the test completes
		// ok to through exceptions from here if need be

//	   if(pid!=0)
//		   kill(pid, SIGKILL);
		//fmu.freeInstance(FMUTest::gcomp);
	}

	~FMUTest()
	{
		// cleanup any pending stuff, but no exceptions allowed
	}

//   int pid;
	// put in any custom data members that you need

	static FMU s_fmu;
	static bool s_libLoaded;

	FMU fmu;
	static fmi2Component gcomp;

private:
	FMU setup()
	{
		if (FMUTest::s_libLoaded)
			return FMUTest::s_fmu;

		HMODULE h;

		//write config
		std::ofstream myfile("config.txt", std::ios::out | std::ios::trunc);
		if (myfile.is_open())
		{
			myfile << "false\n";
#ifdef TEST_RUNNER_DRIVER_PATH
			myfile << TEST_RUNNER_DRIVER_PATH;
#endif
			myfile << "\n";
			myfile.close();
		}

		EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));

		FMUTest::s_fmu = fmu;
		FMUTest::s_libLoaded = true;

		//	if (t1 == NULL)
		//		{
		//			t1 = new std::thread(remoteClientThread);
		//			t1->detach();
		//		}

		return fmu;
	}

public:

	fmi2Component instantiated()
	{
		if (FMUTest::gcomp == NULL)
		{

			char * cwd = getcwd(NULL, 0);

			if (cwd == NULL)
				perror("unable to obtaining cur directory\n");

			FMUTest::gcomp = fmu.instantiate(INSTANCE_NAME, fmi2CoSimulation,
					GUID, cwd, NULL, true, true);
			delete cwd;
		}

		return FMUTest::gcomp;
	}

	fmi2Component setuped()
	{
		fmi2Component comp = instantiated();

		fmi2Status status = fmu.setupExperiment(comp, toleranceDefined,
				tolerance, startTime, stopTimeDefined, stopTime);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}

	fmi2Component initializing()
	{
		fmi2Component comp = setuped();

		fmi2Status status = fmu.enterInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}

	fmi2Component initialized()
	{
		fmi2Component comp = setuped();

		fmi2Status status = fmu.enterInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);

		status = fmu.exitInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}
};

#endif /* FMU_TESTSFIXTURE_H_ */
