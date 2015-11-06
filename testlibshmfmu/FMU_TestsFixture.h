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
	}

	void SetUp()
	{
		fmu = setup();
	}

	void TearDown()
	{
	}

	~FMUTest()
	{
		// cleanup any pending stuff, but no exceptions allowed
	}

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
