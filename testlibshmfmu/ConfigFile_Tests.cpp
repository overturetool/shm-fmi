/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */
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
