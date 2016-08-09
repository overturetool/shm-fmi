/*
 * FmuContainer.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: parallels
 */

#include "FmuContainer.h"

FmuContainer::FmuContainer(FmuProxy *proxy, const char* name, const fmi2CallbackFunctions *functions, JavaLauncher* launcher)
{

  this->m_functions=functions;
  this->m_proxy=proxy;
  this->m_name=name;
  this->m_javaLauncher = launcher;
}

FmuContainer::~FmuContainer()
{
	m_javaLauncher->terminate();
	delete this->m_javaLauncher;
}

