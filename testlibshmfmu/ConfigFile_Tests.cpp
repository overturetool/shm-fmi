#include "gtest/gtest.h"

#include <string>
#include <iostream>

#include "ConfigFile.h"

TEST(ConfigFile, parse) {
  std::string port("8000");

  port += "some name";

  std::string configFile = std::string("config-test.txt");
  configFile.insert(0, "/");
  configFile.insert(0, SOURCE_ROOT);

  printf("Here is source: '%s'\n", configFile.c_str());

  ConfigFile config(configFile, port);

  EXPECT_STREQ("java", config.m_args[0]);
  EXPECT_STREQ("-jar", config.m_args[1]);
  EXPECT_STREQ("fmi-interpreter-0.0.1-SNAPSHOT-jar-with-dependencies.jar",
               config.m_args[2]);
}
