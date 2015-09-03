/*
 * FmuContainer.cpp
 *
 *  Created on: Sep 1, 2015
 *      Author: parallels
 */

#include "FmuContainer.h"

FmuContainer::FmuContainer(ExternalClient *client, const fmi2CallbackFunctions *functions)
{

  this->m_functions=functions;
  this->m_client=client;
}

FmuContainer::~FmuContainer()
{
	// TODO Auto-generated destructor stub
}

