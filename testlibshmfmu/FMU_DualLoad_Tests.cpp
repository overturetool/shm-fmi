#include "gtest/gtest.h"

#include <string>
#include <iostream>
#include "FMULoadHelper.h"

//set
#define realId 1

/*******************************************
 *
 * start/end
 *
 *******************************************/

// This test is the last since it unloads the global test fmu
TEST(FMUDualLoadTest, doubleLoad)
{

	FMULoadHelper loaderFmu1;

	FMU fmu1= loaderFmu1.load();
	fmi2Component compf1i1 =loaderFmu1.initializing("f1 i1");

	FMULoadHelper loaderFmu2;

	FMU fmu2= loaderFmu2.load();
	fmi2Component compf2i1 =loaderFmu1.initializing("f2 i1");

	//now both FMUs are in initialization mode

	//chain test them
	fmi2Status status=fmi2Fatal;
	double inValue = 1.1;
	double outValue = 0.0;

	const fmi2ValueReference vr[1] =
	{	realId};

	//set on fmu1
	status = fmu1.setReal(compf1i1, vr, 1, new double[1]
			{	inValue});
	EXPECT_EQ(fmi2OK, status);
	//get it back

	double values[1];
	status = fmu1.getReal(compf1i1, vr, 1, values);
	EXPECT_EQ(fmi2OK, status);
	outValue=values[0];
	EXPECT_EQ(inValue, outValue);

	//not second fmu
	//set on fmu1
	status = fmu2.setReal(compf2i1, vr, 1, new double[1]
			{	inValue});
	EXPECT_EQ(fmi2OK, status);
	//get it back

	status = fmu2.getReal(compf2i1, vr, 1, values);
	EXPECT_EQ(fmi2OK, status);
	outValue=values[0];
	EXPECT_EQ(inValue, outValue);

//	fmi2Component comp = instantiated();
//
//	fmu.freeInstance(comp);
}
