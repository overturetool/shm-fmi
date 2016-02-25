/*
 * testrunner.cpp
 *
 *  Created on: Nov 5, 2015
 *      Author: parallels
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
