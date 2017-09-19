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
  JavaLauncher(bool visible, const char* workingDir, char** args);
  virtual ~JavaLauncher();
  int launch();
  void terminate();

  static bool debug;
  static debugPrintType* debugPrintPtr;

 private:
  const char* m_workingDir;
  char** m_args;
  bool m_launched;
  bool m_visible;

#ifdef _WIN32
  PROCESS_INFORMATION pid;

#elif __APPLE__ || __linux
  pid_t pid;
#endif
};

#endif /* JAVALAUNCHER_H_ */
