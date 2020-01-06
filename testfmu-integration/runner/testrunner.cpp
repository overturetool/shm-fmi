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


#include <string>
#include <iostream>

#include "../RemoteTestDriver.h"
#include <thread>
#include <chrono>



#define toleranceDefined true
#define tolerance 0.1
#define startTime 0.0
#define stopTimeDefined true
#define stopTime 1.0

//set
#define realId 1
#define intId 2
#define boolId 3
#define stringId 4

//get
#define realIdGet 10
#define intIdGet 11
#define boolIdGet 12
#define stringIdGet 13

#define GUID "{GUID}"
#define INSTANCE_NAME "instance1"

int main (int argc, char *argv[]) {

	const char* name ="shmFmiTest";

	if(argc>1)
		name = argv[1];

#ifndef REMOTE_TEST_DRIVER
	while (true){
		//printf("run test driver with key: '%s'\n",name);
		remoteTestDriver(name);}
#endif

}
