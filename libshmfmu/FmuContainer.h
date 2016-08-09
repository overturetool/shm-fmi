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


class FmuContainer
{
public:
	FmuContainer(FmuProxy *proxy, const char* name, const fmi2CallbackFunctions *functions,JavaLauncher* launcher);
	virtual ~FmuContainer();

public:
	const fmi2CallbackFunctions *m_functions;
	FmuProxy* m_proxy;
	const char* m_name;
	JavaLauncher *m_javaLauncher;
};

#endif /* FMUCONTAINER_H_ */
