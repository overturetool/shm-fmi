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

#ifndef FMUCONTAINER_H_
#define FMUCONTAINER_H_

#include "FmuProxy.h"
#include "fmi2Functions.h"
#include "JavaLauncher.h"
#include "SharedFmiMessage.h"
#include <thread>

class FmuContainer {
 public:
  FmuContainer(int id, FmuProxy* proxy, const char* name,
               const fmi2CallbackFunctions* functions, JavaLauncher* launcher);
  virtual ~FmuContainer();

 public:
  const fmi2CallbackFunctions* m_functions;
  FmuProxy* m_proxy;
  const char* m_name;
  JavaLauncher* m_javaLauncher;
  bool active;

  // FMI

  fmi2ComponentEnvironment componentEnvironment;

  int id;
  std::thread* callbackThread;
};

#endif /* FMUCONTAINER_H_ */
