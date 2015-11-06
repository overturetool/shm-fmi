/*
 * FMULoadHelper.h
 *
 *  Created on: Nov 6, 2015
 *      Author: parallels
 */

#ifndef FMULOADHELPER_H_
#define FMULOADHELPER_H_

#include "gtest/gtest.h"

//file io
#include <iostream>
#include <fstream>

//current dir
#include <unistd.h>

extern "C"
{
#include "fmu-loader.h"
}


#define toleranceDefined true
#define tolerance 0.1
#define startTime 0.0
#define stopTimeDefined true
#define stopTime 1.0

#define GUID "{GUID}"

class FMULoadHelper
{
private:
	FMU fmu;

public:
	FMU load()
	{

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

		return fmu;
	}

	fmi2Component instantiate(const char* name)
	{
		char * cwd = getcwd(NULL, 0);

		if (cwd == NULL)
		perror("unable to obtaining cur directory\n");

		fmi2Component comp = fmu.instantiate(name, fmi2CoSimulation,
				GUID, cwd, NULL, true, true);
		delete cwd;

		return comp;
	}

	fmi2Component setuped(const char* name)
	{
		fmi2Component comp = instantiate(name);

		fmi2Status status = fmu.setupExperiment(comp, toleranceDefined,
				tolerance, startTime, stopTimeDefined, stopTime);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}

	fmi2Component initializing(const char* name)
	{
		fmi2Component comp = setuped(name);

		fmi2Status status = fmu.enterInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}

	fmi2Component initialized(const char* name)
	{
		fmi2Component comp = setuped(name);

		fmi2Status status = fmu.enterInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);

		status = fmu.exitInitializationMode(comp);

		EXPECT_EQ(fmi2OK, status);
		return comp;
	}
};

#endif /* FMULOADHELPER_H_ */
