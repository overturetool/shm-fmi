#include "gtest/gtest.h"

#include <string>
#include <iostream>


#include "ConfigFile.h"



TEST(ConfigFile, parse)
{
	std::string port("8000");

		port += "some name";

//		std::string resourceLocationStr(fmuResourceLocation);

		std::string configFile =  std::string("config-test.txt");
		ConfigFile config(configFile, port);

		EXPECT_STREQ("java", config.m_args[0]);
		EXPECT_STREQ("-jar", config.m_args[1]);
		EXPECT_STREQ("fmi-interpreter-0.0.1-SNAPSHOT-jar-with-dependencies.jar", config.m_args[2]);
}

