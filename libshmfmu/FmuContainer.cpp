/*
 * FmuContainer.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: parallels
 */

#include "FmuContainer.h"

FmuContainer::FmuContainer(int id, FmuProxy *proxy, const char* name, const fmi2CallbackFunctions *functions,
		JavaLauncher* launcher)
{
	this->m_functions = functions;
	this->m_proxy = proxy;

	this->m_name = (char *) calloc(sizeof(char), strlen(name) + 1);
	strcpy((char*) m_name, name);

	this->m_javaLauncher = launcher;
	this->active = true;
	this->componentEnvironment = functions->componentEnvironment;
	this->id = id;
	this->callbackThread = NULL;
}

FmuContainer::~FmuContainer()
{
	m_javaLauncher->terminate();
	delete this->m_javaLauncher;
	this->m_javaLauncher = NULL;
	delete this->m_name;
	this->m_name = NULL;
	delete this->m_proxy;
	this->m_proxy = NULL;
	if (this->callbackThread != NULL)
	{
		delete this->callbackThread;
		this->callbackThread = NULL;
	}
}

