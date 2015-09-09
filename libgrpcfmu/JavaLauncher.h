/*
 * JavaLauncher.h
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#ifndef JAVALAUNCHER_H_
#define JAVALAUNCHER_H_

#include <iostream>

#include <string>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <unistd.h>

//kill
#include <sys/types.h>
#include <signal.h>

//sleep
#include <unistd.h>


	#include <errno.h>

class JavaLauncher
{
public:
	JavaLauncher( char** args);
	virtual ~JavaLauncher();
	int launch();
	void terminate();
private:

	 char **m_args;
	bool m_launched;
	pid_t pid;
};

#endif /* JAVALAUNCHER_H_ */
