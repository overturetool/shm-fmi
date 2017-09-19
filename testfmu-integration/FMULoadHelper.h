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

#ifndef FMULOADHELPER_H_
#define FMULOADHELPER_H_

#include "gtest/gtest.h"

// file io
#include <iostream>
#include <fstream>

#include "resource_location.h"

extern "C" {
#include "fmu-loader.h"
}
#include "fmi2FunctionTypes.h"

extern "C" {
void fmuLogger(fmi2ComponentEnvironment, fmi2String, fmi2Status, fmi2String,
               fmi2String, ...);
}

#define toleranceDefined true
#define tolerance 0.1
#define startTime 0.0
#define stopTimeDefined true
#define stopTime 1.0

#define GUID "{GUID}"

class FMULoadHelper {
 private:
  FMU fmu;
  fmi2CallbackFunctions* m_callback = NULL;
  fmi2Component comp = NULL;

 public:
  ~FMULoadHelper() {
    // cleanup any pending stuff, but no exceptions allowed
    delete m_callback;
  }

  FMU load() {
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

  fmi2Component instantiate(const char* name) {
    std::string* cwd = getResourceLocation();

    m_callback = (fmi2CallbackFunctions*)malloc(sizeof(fmi2CallbackFunctions));
    fmi2CallbackFunctions st = {&fmuLogger, NULL, NULL, NULL, cwd};
    memcpy(m_callback, &st, sizeof(fmi2CallbackFunctions));

    comp = fmu.instantiate(name, fmi2CoSimulation, GUID, cwd->c_str(),
                           m_callback, true, true);

    delete cwd;

    return comp;
  }

  fmi2Component setuped(const char* name) {
    fmi2Component comp = instantiate(name);

    fmi2Status status =
        fmu.setupExperiment(comp, toleranceDefined, tolerance, startTime,
                            stopTimeDefined, stopTime);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }

  fmi2Component initializing(const char* name) {
    fmi2Component comp = setuped(name);

    fmi2Status status = fmu.enterInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }

  fmi2Component initialized(const char* name) {
    fmi2Component comp = setuped(name);

    fmi2Status status = fmu.enterInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);

    status = fmu.exitInitializationMode(comp);

    EXPECT_EQ(fmi2OK, status);
    return comp;
  }
};

#endif /* FMULOADHELPER_H_ */
