/*
 * FmuContainer.h
 *
 *  Created on: Aug 26, 2015
 *      Author: parallels
 */

#ifndef FMUCONTAINER_H_
#define FMUCONTAINER_H_

#include "FmuProxy.h"
#include "fmi2Functions.h"
#include "JavaLauncher.h"
#include "SharedFmiMessage.h"
#include <thread>

class FmuContainer
{
public:
	FmuContainer(int id, FmuProxy *proxy, const char* name, const fmi2CallbackFunctions *functions, JavaLauncher* launcher);
	virtual ~FmuContainer();

public:
	const fmi2CallbackFunctions *m_functions;
	FmuProxy* m_proxy;
	const char* m_name;
	JavaLauncher *m_javaLauncher;
	bool active;

	//FMI

	fmi2ComponentEnvironment componentEnvironment;

	int id;
	std::thread* callbackThread;
};

#endif /* FMUCONTAINER_H_ */
