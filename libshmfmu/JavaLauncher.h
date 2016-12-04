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
#include <stdio.h>  /* printf */
#include <stdlib.h> /* system, NULL, EXIT_FAILURE */
#include <unistd.h>

// kill
#include <sys/types.h>
#include <signal.h>

// sleep
#include <unistd.h>

#include <errno.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>

#elif __APPLE__ || __linux
#include <cstring>
#include <sys/wait.h>
#endif

class JavaLauncher {
 public:
  typedef int debugPrintType(void* sender, const char* format, ...);
  JavaLauncher(const char* workingDir, char** args);
  virtual ~JavaLauncher();
  int launch();
  void terminate();

  static bool debug;
  static debugPrintType* debugPrintPtr;

 private:
  const char* m_workingDir;
  char** m_args;
  bool m_launched;

#ifdef _WIN32
  PROCESS_INFORMATION pid;

#elif __APPLE__ || __linux
  pid_t pid;
#endif
};

#endif /* JAVALAUNCHER_H_ */
