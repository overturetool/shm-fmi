/*
 * JavaLauncher.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#include "JavaLauncher.h"

JavaLauncher::JavaLauncher(const char* workingDir, char** args) {
	this->m_workingDir = workingDir;
	this->m_launched = false;
	this->m_args = args;

#ifdef _WIN32

#elif __APPLE__ ||  __linux
	this->pid = -1;
#endif
}

JavaLauncher::~JavaLauncher() {

}

#ifdef _WIN32
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

// Create a string with last error message
std::string GetLastErrorStdStr()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr+bufLen);

      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}
#endif

int JavaLauncher::launch() {
#ifdef _WIN32

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	std::string cmd = "";

	int i = 0;
	while (m_args[i] != NULL) {

		//if (i > 0) {
			cmd += m_args[i];
			cmd += " ";
	//	}
		i++;
	}

	int kk = cmd.length();
		char * buf = new char[cmd.length()];
		memcpy(buf,cmd.c_str(),cmd.length());
		buf[kk-1]=NULL;

	printf("Launching process: '%s' with arguments: '%s' in folder '%s'\n",m_args[0],buf,m_workingDir);

	// Start the child process.
	if (!CreateProcess(NULL,//m_args[0],   // No module name (use command line)
			buf,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			m_workingDir,           // Use parent's starting directory
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			) {
		//printf("CreateProcess failed (%d).\n", GetLastError());
		printf("---CresteProcess failed %s\n",GetLastErrorStdStr().c_str());
		delete buf;
		return 1;
	}
	delete buf;
printf("Process launched continuing main thread.\n");
	return 0;
#elif __APPLE__ ||  __linux

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
#endif
}

void JavaLauncher::terminate() {
	if (m_launched) {
#ifdef _WIN32
		UINT uExitCode = 0;
		TerminateProcess(pid.hProcess, uExitCode);

#elif __APPLE__ ||  __linux
		kill(pid, SIGKILL);
#endif
	}
}
