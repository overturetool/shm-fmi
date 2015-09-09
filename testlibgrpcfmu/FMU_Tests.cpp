#include "gtest/gtest.h"

#include <string>
#include <iostream>

extern "C"
{
#include "fmu-loader.h"
}

#define toleranceDefined true
#define tolerance 0.1
#define startTime 0.0
#define stopTimeDefined true
#define stopTime 1.0

//set
#define realId 1
#define intId 2
#define boolId 3
#define stringId 4

//get
#define realIdGet 10
#define intIdGet 11
#define boolIdGet 12
#define stringIdGet 13

static FMU s_fmu;
static bool s_libLoaded = false;

static FMU setup()
{
	if (s_libLoaded)
		return s_fmu;

	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));

	s_fmu = fmu;
	s_libLoaded = true;
	return fmu;
}

static fmi2Component instantiated(FMU fmu)
{
	return fmu.instantiate("A", fmi2CoSimulation, "{348783748923}", ".", NULL,
			true, true);
}

static fmi2Component setuped(FMU fmu)
{
	fmi2Component comp = instantiated(fmu);

	fmi2Status status = fmu.setupExperiment(comp, toleranceDefined, tolerance,
	startTime, stopTimeDefined, stopTime);

	EXPECT_EQ(fmi2OK, status);
	return comp;
}

static fmi2Component initializing(FMU fmu)
{
	fmi2Component comp = setuped(fmu);

	fmi2Status status = fmu.enterInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
	return comp;
}

static fmi2Component initialized(FMU fmu)
{
	fmi2Component comp = setuped(fmu);

	fmi2Status status = fmu.enterInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);

	status = fmu.exitInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
	return comp;
}

/*******************************************
 *
 * start/end
 *
 *******************************************/

TEST(FMU, getTypesPlatform)
{
	FMU fmu = setup();
	fmu.getTypesPlatform();
}

TEST(FMU, getVersion)
{
	FMU fmu = setup();
	EXPECT_STREQ("2.0", fmu.getVersion());
}

TEST(FMU, instantiate)
{
	FMU fmu = setup();
	fmu.instantiate("A", fmi2CoSimulation, "{348783748923}", ".", NULL, true,
			true);
}

/*******************************************
 *
 * instantiated
 *
 *******************************************/
TEST(FMU, setDebugLogging)
{
	FMU fmu = setup();
	fmi2Component comp = instantiated(fmu);

	const char* categories[] =
	{ "all", "debug" };
	fmi2Status status = fmu.setDebugLogging(comp, true, 2, categories); //fmi2Boolean, size_t, const fmi2String[]

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, freeInstance)
{
	FMU fmu = setup();
	fmi2Component comp = instantiated(fmu);

	fmu.freeInstance(comp);
}

TEST(FMU, setupExperiment)
{
	FMU fmu = setup();
	fmi2Component comp = instantiated(fmu);

	fmi2Status status = fmu.setupExperiment(comp, toleranceDefined, tolerance,
	startTime, stopTimeDefined, stopTime);

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, reset)
{
	FMU fmu = setup();
	fmi2Component comp = instantiated(fmu);

	fmi2Status status = fmu.reset(comp);

	EXPECT_EQ(fmi2OK, status);
}

/*******************************************
 *
 * setup. Note that this state doesn't exists in the 2.0 standard but it should and is reported
 *
 *******************************************/

TEST(FMU, enterInitializationMode)
{
	FMU fmu = setup();
	fmi2Component comp = setuped(fmu);

	fmi2Status status = fmu.enterInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, setReal)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	const fmi2ValueReference vr[1] =
	{ realId };
	fmi2Status status = fmu.setReal(comp, vr, 1, new double[1]
	{ 1.1 });

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, setInteger)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	const fmi2ValueReference vr[1] =
	{ intId };
	fmi2Status status = fmu.setInteger(comp, vr, 1, new int[1]
	{ 1 });

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, setString)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	const fmi2ValueReference vr[1] =
	{ stringId };
	fmi2Status status = fmu.setString(comp, vr, 1, new fmi2String[1]
	{ "test" });

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, setRealInputDerivatives)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	const fmi2ValueReference vr[1] =
	{ stringId };
	fmi2Status status = fmu.setRealInputDerivatives(comp, vr, 1, new int[1]
	{ 1 }, new double[1]
	{ 1.1 });

	//not FW'ed to receiver
	EXPECT_EQ(fmi2Error, status);
}

// State stuff

TEST(FMU, getsetFMUstate)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	fmi2FMUstate* state;
	fmi2Status status = fmu.getFMUstate(comp, state);

	//not FW'ed to receiver
	EXPECT_EQ(fmi2Error, status);

	if (status == fmi2OK)
	{
		status = fmu.setFMUstate(comp, state);
		EXPECT_EQ(fmi2OK, status);

		status = fmu.freeFMUstate(comp, state);
		EXPECT_EQ(fmi2OK, status);
	}
}

/*******************************************
 *
 * initializing
 *
 *******************************************/

TEST(FMU, exitInitializationMode)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);

	fmi2Status status = fmu.exitInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, getReal)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);

	const fmi2ValueReference vr[1] =
	{ realIdGet };

	double values[1];
	fmi2Status status = fmu.getReal(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(9.9, values[0]);
}

TEST(FMU, getBoolean)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);

	const fmi2ValueReference vr[1] =
	{ boolIdGet };

	fmi2Boolean values[1];
	fmi2Status status = fmu.getBoolean(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(true, values[0]);
}

TEST(FMU, getInteger)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);

	const fmi2ValueReference vr[1] =
	{ intIdGet };

	int values[1];
	fmi2Status status = fmu.getInteger(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(1, values[0]);
}

TEST(FMU, getString)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);

	const fmi2ValueReference vr[1] =
	{ stringIdGet };

	const char* values[1];
	fmi2Status status = fmu.getString(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_TRUE(values[0] != 0);
	EXPECT_STREQ("undefined", values[0]);
}

TEST(FMU, getDirectionalDerivative)
{
	FMU fmu = setup();
	fmi2Component comp = initializing(fmu);
	const fmi2ValueReference vr[1] =
	{ stringId };

	fmi2Real values[1];

	fmi2Status status = fmu.getDirectionalDerivative(comp, vr, 1, vr, 1,
			new double[1]
			{ 1.1 }, values);

	/*
	 *    typedef fmi2Status fmi2GetDirectionalDerivativeTYPE(fmi2Component, const fmi2ValueReference[], size_t,
	 const fmi2ValueReference[], size_t,
	 const fmi2Real[], fmi2Real[]);*/
	//not FW'ed to receiver
	EXPECT_EQ(fmi2Error, status);
}

/*******************************************
 *
 * initialized
 *
 *******************************************/

TEST(FMU, terminate)
{
	FMU fmu = setup();
	fmi2Component comp = initialized(fmu);

	fmi2Status status = fmu.terminate(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST(FMU, doStep)
{
	FMU fmu = setup();
	fmi2Component comp = initialized(fmu);

	fmi2Status status = fmu.doStep(comp, 0.0, 1.0, false);

	EXPECT_EQ(fmi2OK, status);
}

//TODO: getRealOutputDerivatives

//INTO specific

TEST(FMU, getMaxStepsize)
{
	FMU fmu = setup();
	fmi2Component comp = initialized(fmu);

	if (fmu.getMaxStepsize != NULL)
	{
		double size;
		fmi2Status status = fmu.getMaxStepsize(comp, &size);
		EXPECT_EQ(fmi2OK, status);
		EXPECT_EQ(100, size);
	}
}
