/*
 * JavaLauncher.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#include "JavaLauncher.h"

bool JavaLauncher::debug = true;

int javaLauncherInternalDebugPrint(void* sender, const char * format, ...)
{
	if (!JavaLauncher::debug)
	{
		return 0;
	}
	va_list args;
	va_start(args, format);
	int ret = vfprintf(stdout, format, args);
	va_end(args);
	return ret;
}

JavaLauncher::debugPrintType* JavaLauncher::debugPrintPtr = &javaLauncherInternalDebugPrint;

JavaLauncher::JavaLauncher(const char* workingDir, char** args)
{
	this->m_workingDir = workingDir;
	this->m_launched = false;
	this->m_args = args;

#ifdef _WIN32

#elif __APPLE__ ||  __linux
	this->pid = -1;
#endif
}

JavaLauncher::~JavaLauncher()
{

}

#ifdef _WIN32
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0)
	return std::string();//No error message has been recorded

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

int JavaLauncher::launch()
{
#ifdef _WIN32

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	std::string cmd = "";

	int i = 0;
	while (m_args[i] != NULL)
	{
		cmd += m_args[i];
		cmd += " ";
		i++;
	}

	int kk = cmd.length();
	char * buf = new char[cmd.length()];
	memcpy(buf,cmd.c_str(),cmd.length());
	buf[kk-1]=0;

	debugPrintPtr(this,"Launching process: '%s' with arguments: '%s' in folder '%s'\n",m_args[0],buf,m_workingDir);

	// Start the child process.
	if (!CreateProcess(NULL,//m_args[0],   // No module name (use command line)
					buf,// Command line
					NULL,// Process handle not inheritable
					NULL,// Thread handle not inheritable
					FALSE,// Set handle inheritance to FALSE
					CREATE_NO_WINDOW,// No creation flags
					NULL,// Use parent's environment block
					m_workingDir,// Use parent's starting directory
					&si,// Pointer to STARTUPINFO structure
					&pi)// Pointer to PROCESS_INFORMATION structure
	)
	{
		//printf("CreateProcess failed (%d).\n", GetLastError());
		debugPrintPtr(this,"---CresteProcess failed %s\n",GetLastErrorStdStr().c_str());
		delete buf;
		return 1;
	}
	delete buf;
	debugPrintPtr(this,"Process launched continuing main thread.\n");
	this->pid=pi;
	this->m_launched=true;
	return 0;
#elif __APPLE__ ||  __linux

	int status;
	pid_t pid2;
	pid = fork();
	if (pid == 0)
	{

		/* child process B */
		pid2 = fork();
		if (pid2 )
		{
			exit(0);
		} else if (!pid2)
		{
			/* child process C */
			debugPrintPtr(this, "Fork application %s\n", m_args[0]);
			if (m_workingDir != NULL)
			{
				if (chdir(m_workingDir) == 0)
				{
					debugPrintPtr(this, "Changed dir to: %s\n", m_workingDir);
				} else
				{
					debugPrintPtr(this, "server_create: failed to fork and change dir: %01d %s\n",
					__LINE__, strerror( errno));
				}

			}
			int i = 0;
			while (m_args[i] != NULL)
			{
				debugPrintPtr(this, "%s\n", m_args[i]);
				i++;
			}

			status = execvp(m_args[0], m_args);
		} else
		{
			/* error */
		}

	} else if (pid == -1)
	{
		// Parent process will return a non-zero value from fork()
		debugPrintPtr(this, "server_create: failed to fork: %01d %s\n", __LINE__, strerror( errno));

		m_launched = false;
	} else
	{
		m_launched = true;
		/* parent process A */
		waitpid(pid, &status, 0);
	}

	debugPrintPtr(this, "Fork pid %d\n", pid);

	return status;
#endif
}

void JavaLauncher::terminate()
{
	if (m_launched)
	{
#ifdef _WIN32
		UINT uExitCode = 0;
		TerminateProcess(pid.hProcess, uExitCode);

#elif __APPLE__ ||  __linux
		//kill(pid, SIGKILL);
#endif
		m_launched = false;
	}
}
