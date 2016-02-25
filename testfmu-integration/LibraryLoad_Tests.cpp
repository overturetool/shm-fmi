#include "gtest/gtest.h"

#include <string>
#include <iostream>

extern "C"
{
#include "fmu-loader.h"
}


TEST(LibraryLoad, loadsingle)
{
	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}

TEST(LibraryLoad, loaddouble)
{
	FMU fmu;
	HMODULE h;

	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
	EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}
