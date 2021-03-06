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

#ifndef FMU_H_
#define FMU_H_

#include "gtest/gtest.h"

// file io
#include <iostream>
#include <fstream>

#include "resource_location.h"

// current dir
#include <unistd.h>

#include "fmi2FunctionTypes.h"

extern "C" {
void fmuLogger(fmi2ComponentEnvironment, fmi2String, fmi2Status, fmi2String,
               fmi2String, ...);
}

class FMUTest : public ::testing::Test {
 public:
  FMUTest() {
    comp = NULL;
    m_callback = NULL;
  }

  void SetUp() { fmu = setup(); }

  void TearDown() {
    if (fmu.dllHandle != NULL && comp != NULL) {
      if (isInstantiated) {
        fmu.freeInstance(comp);
      }
      fmu.freeInstance(comp);
    }
  }

  ~FMUTest() {
    // cleanup any pending stuff, but no exceptions allowed
    if (this->m_callback != NULL) delete m_callback;
  }

  FMU fmu;
  fmi2Component comp;
  fmi2CallbackFunctions* m_callback;

  bool isInstantiated = false;

 private:
  FMU setup() {
    HMODULE h;

    // write config
    std::ofstream myfile("config.txt", std::ios::out | std::ios::trunc);
    if (myfile.is_open()) {
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

 public:
  fmi2Component instantiated() {
    std::string* cwd = getResourceLocation();

    m_callback = (fmi2CallbackFunctions*)malloc(sizeof(fmi2CallbackFunctions));
    fmi2CallbackFunctions st = {&fmuLogger, NULL, NULL, NULL, cwd};
    memcpy(m_callback, &st, sizeof(fmi2CallbackFunctions));

    comp = fmu.instantiate(INSTANCE_NAME, fmi2CoSimulation, GUID, cwd->c_str(),
                           m_callback, true, true);
    delete cwd;

    isInstantiated = true;
    return comp;
  }

  fmi2Component setuped() {
    fmi2Component comp = instantiated();

    fmi2Status status =
        fmu.setupExperiment(comp, toleranceDefined, tolerance, startTime,
                            stopTimeDefined, stopTime);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }

  fmi2Component initializing() {
    fmi2Component comp = setuped();

    fmi2Status status = fmu.enterInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }

  fmi2Component initialized() {
    fmi2Component comp = setuped();

    fmi2Status status = fmu.enterInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);

    status = fmu.exitInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }
};

#endif /* FMU_TESTSFIXTURE_H_ */
