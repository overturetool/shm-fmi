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

#include "FmuContainer.h"

FmuContainer::FmuContainer(int id, FmuProxy *proxy, const char *name,
                           const fmi2CallbackFunctions *functions,
                           JavaLauncher *launcher) {
  this->m_functions = functions;
  this->m_proxy = proxy;

  this->m_name = (char *)calloc(sizeof(char), strlen(name) + 1);
  strcpy((char *)m_name, name);

  this->m_javaLauncher = launcher;
  this->active = true;
  this->componentEnvironment = functions->componentEnvironment;
  this->id = id;
  this->callbackThread = NULL;
}

FmuContainer::~FmuContainer() {
  m_javaLauncher->terminate();
  delete this->m_javaLauncher;
  this->m_javaLauncher = NULL;
  delete this->m_name;
  this->m_name = NULL;
  delete this->m_proxy;
  this->m_proxy = NULL;
  if (this->callbackThread != NULL) {
    delete this->callbackThread;
    this->callbackThread = NULL;
  }
}
