/*
 * FmuContainer.h
 *
 *  Created on: Aug 26, 2015
 *      Author: parallels
 */

#ifndef FMUCONTAINER_H_
#define FMUCONTAINER_H_

#include "ExternalClient.h"
#include "fmi2Functions.h"


class FmuContainer
{
public:
	FmuContainer(ExternalClient *client, const fmi2CallbackFunctions *functions);
	virtual ~FmuContainer();

public:
	const fmi2CallbackFunctions *m_functions;
	ExternalClient* m_client;
};

#endif /* FMUCONTAINER_H_ */
