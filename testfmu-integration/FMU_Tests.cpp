#include "gtest/gtest.h"

#include <string>
#include <iostream>

//#include "RemoteTestDriver.h"
#include <thread>
#include <chrono>

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

#define GUID "{GUID}"
#define INSTANCE_NAME "instance1"


#include "FMU_TestsFixture.h"


/*******************************************
 *
 * start/end
 *
 *******************************************/

TEST_F(FMUTest, getTypesPlatform)
{
	fmu.getTypesPlatform();
}

TEST_F(FMUTest, getVersion)
{
	EXPECT_STREQ("2.0", fmu.getVersion());
}

TEST_F(FMUTest, instantiate)
{
	instantiated();
}

/*******************************************
 *
 * instantiated
 *
 *******************************************/
TEST_F(FMUTest, setDebugLogging)
{
	fmi2Component comp = instantiated();

	const char* categories[] =
	{ "all", "debug" };
	fmi2Status status = fmu.setDebugLogging(comp, true, 2, categories); //fmi2Boolean, size_t, const fmi2String[]

	EXPECT_EQ(fmi2OK, status);
}

//TEST_F(FMUTest, freeInstance)
//{
//	fmi2Component comp = instantiated();
//
//	//fmu.freeInstance(comp);
//}

TEST_F(FMUTest, setupExperiment)
{
	fmi2Component comp = instantiated();

	fmi2Status status = fmu.setupExperiment(comp, toleranceDefined, tolerance,
	startTime, stopTimeDefined, stopTime);

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, reset)
{
	fmi2Component comp = instantiated();

	fmi2Status status = fmu.reset(comp);

	EXPECT_EQ(fmi2OK, status);
}

/*******************************************
 *
 * setup. Note that this state doesn't exists in the 2.0 standard but it should and is reported
 *
 *******************************************/

TEST_F(FMUTest, enterInitializationMode)
{
	fmi2Component comp = setuped();

	fmi2Status status = fmu.enterInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, setReal)
{
	fmi2Component comp = initializing();
	const fmi2ValueReference vr[1] =
	{ realId };
	fmi2Status status = fmu.setReal(comp, vr, 1, new double[1]
	{ 1.1 });

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, setInteger)
{
	fmi2Component comp = initializing();
	const fmi2ValueReference vr[1] =
	{ intId };
	fmi2Status status = fmu.setInteger(comp, vr, 1, new int[1]
	{ 1 });

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, setString)
{
	fmi2Component comp = initializing();
	const fmi2ValueReference vr[1] =
	{ stringId };
	fmi2Status status = fmu.setString(comp, vr, 1, new fmi2String[1]
	{ "test" });

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, setRealInputDerivatives)
{
	fmi2Component comp = initializing();
	//printf("before setRealInputDerivatives\n");
	const fmi2ValueReference vr[1] =
	{ stringId };
	fmi2Status status = fmu.setRealInputDerivatives(comp, vr, 1, new int[1]
	{ 1 }, new double[1]
	{ 1.1 });
//	printf("after setRealInputDerivatives\n");
//not FW'ed to receiver
	EXPECT_EQ(fmi2Error, status);
}

// State stuff

TEST_F(FMUTest, getsetFMUstate)
{
	fmi2Component comp = initializing();
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

TEST_F(FMUTest, exitInitializationMode)
{
	fmi2Component comp = initializing();

	fmi2Status status = fmu.exitInitializationMode(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, getReal)
{
	fmi2Component comp = initializing();

	const fmi2ValueReference vr[1] =
	{ realIdGet };

	double values[1];
	fmi2Status status = fmu.getReal(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(9.9, values[0]);
}

TEST_F(FMUTest, getBoolean)
{
	fmi2Component comp = initializing();

	const fmi2ValueReference vr[1] =
	{ boolIdGet };

	fmi2Boolean values[1];
	fmi2Status status = fmu.getBoolean(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(true, values[0]);
}

TEST_F(FMUTest, getInteger)
{
	fmi2Component comp = initializing();

	const fmi2ValueReference vr[1] =
	{ intIdGet };

	int values[1];
	fmi2Status status = fmu.getInteger(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_EQ(1, values[0]);
}

TEST_F(FMUTest, getString)
{
	fmi2Component comp = initializing();

	const fmi2ValueReference vr[1] =
	{ stringIdGet };

	const char* values[1];
	fmi2Status status = fmu.getString(comp, vr, 1, values);

	EXPECT_EQ(fmi2OK, status);

	EXPECT_TRUE(values[0] != 0);
	EXPECT_STREQ("undefined", values[0]);
}

TEST_F(FMUTest, getDirectionalDerivative)
{
	fmi2Component comp = initializing();
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

TEST_F(FMUTest, terminate)
{
	fmi2Component comp = initialized();

	fmi2Status status = fmu.terminate(comp);

	EXPECT_EQ(fmi2OK, status);
}

TEST_F(FMUTest, doStep)
{
	fmi2Component comp = initialized();

	fmi2Status status = fmu.doStep(comp, 0.0, 1.0, false);

	EXPECT_EQ(fmi2OK, status);
}

//TODO: getRealOutputDerivatives

//INTO specific

TEST_F(FMUTest, getMaxStepsize)
{
	fmi2Component comp = initialized();

	if (fmu.getMaxStepsize != NULL)
	{
		double size;
		fmi2Status status = fmu.getMaxStepsize(comp, &size);
		EXPECT_EQ(fmi2OK, status);
		EXPECT_EQ(100, size);
	}
}


//get status
TEST_F(FMUTest, getStatus)
{
	fmi2Component comp = initialized();

	fmi2StatusKind kind = fmi2LastSuccessfulTime;
	fmi2Status value ;

	fmi2Status status = fmu.getStatus(comp,kind,&value);

	EXPECT_EQ(fmi2OK, status);
	EXPECT_EQ(fmi2OK, value);
}

TEST_F(FMUTest, fmi2GetRealStatus)
{
	fmi2Component comp = initialized();

	fmi2StatusKind kind = fmi2LastSuccessfulTime;
	fmi2Real value ;

	fmi2Status status = fmu.getRealStatus(comp,kind,&value);

	EXPECT_EQ(fmi2OK, status);
	EXPECT_EQ(100.5, value);
}

TEST_F(FMUTest, fmi2GetIntegerStatus)
{
	fmi2Component comp = initialized();

	fmi2StatusKind kind = fmi2LastSuccessfulTime;
	fmi2Integer value ;

	fmi2Status status = fmu.getIntegerStatus(comp,kind,&value);

	EXPECT_EQ(fmi2OK, status);
	EXPECT_EQ(100, value);
}

TEST_F(FMUTest, fmi2GetBooleanStatus)
{
	fmi2Component comp = initialized();

	fmi2StatusKind kind = fmi2Terminated;
	fmi2Boolean value ;

	fmi2Status status = fmu.getBooleanStatus(comp,kind,&value);

	EXPECT_EQ(fmi2OK, status);
	EXPECT_EQ(true, value);
}


TEST_F(FMUTest, fmi2GetStringStatus)
{
	fmi2Component comp = initialized();

	fmi2StatusKind kind = fmi2DoStepStatus;
	fmi2String value ;

	fmi2Status status = fmu.getStringStatus(comp,kind,&value);

	EXPECT_EQ(fmi2OK, status);
	ASSERT_STREQ("waiting", value);
}


TEST_F(FMUTest, freeInstance)
{
	fmi2Component comp = instantiated();

	fmu.freeInstance(comp);
	isInstantiated = false;
	//we must tell teardown to skip freeinstance
	FMUTest::comp = NULL;
}
