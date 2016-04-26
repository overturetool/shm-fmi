#include "gtest/gtest.h"

#include <string>
#include <iostream>

#include "ConfigFile.h"
#include "JavaLauncher.h"

TEST(JavaLauncher, launchfail)
{
	std::string port("8000");

	port += "some name";

	std::string configFile = std::string("config-test.txt");
	configFile.insert(0, "/");
	configFile.insert(0, SOURCE_ROOT);

	ConfigFile config(configFile, port);

	JavaLauncher *launcher = new JavaLauncher("/", config.m_args);

	launcher->launch();
	launcher->terminate();
}

TEST(JavaLauncher, launch)
{
	std::string port("8000");

	port += "some name";

	std::string configFile = std::string("config-test-echo.txt");
	configFile.insert(0, "/");
	configFile.insert(0, SOURCE_ROOT);

	ConfigFile config(configFile, port);

	JavaLauncher *launcher = new JavaLauncher("/", config.m_args);

	launcher->launch();
	launcher->terminate();
}

