/*
 * JavaLauncher.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#include "JavaLauncher.h"

JavaLauncher::JavaLauncher(const char* workingDir, char** args)
{
	this->m_workingDir = workingDir;
	this->m_launched = false;
	this->pid = -1;
	this->m_args = args;
}

JavaLauncher::~JavaLauncher()
{

}

int JavaLauncher::launch()
{
	int status;
	if ((pid = fork()) == 0)
	{
		printf("Fork application %s\n", m_args[0]);
		// Child process will return 0 from fork()

		//	status = execlp("/bin/ls", "/bin/ls", "-r", "-t", "-l", (char *) 0);//system(m_path.c_str());

		/*printf("Launching:\n");
		 for(int i =0;i< 5;i++)
		 {
		 printf("%s ",m_args[i]);
		 }
		 printf("\n");*/
		if(m_workingDir!=NULL)
			chdir(m_workingDir);

		status = execvp(m_args[0], m_args);
		printf("Execvp status: %d\n", status);
		printf("Execvp error: %d", errno);
		exit(0);
	} else
	{
		// Parent process will return a non-zero value from fork()

		m_launched = true;
	}

	printf("Fork pid %d\n", pid);

	usleep(0.5 * 1000000);

	return status;
}

void JavaLauncher::terminate()
{
	if (m_launched)
	{
		kill(pid, SIGKILL);
	}
}
