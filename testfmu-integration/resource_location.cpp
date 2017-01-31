/*
 * fmu_working_dir.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: kel
 */
#include "resource_location.h"

#include <algorithm>

// current dir
#include <unistd.h>

std::string* getResourceLocation() {
  const char* cwdPtr = getcwd(NULL, 0);
  if (cwdPtr == NULL) {
    perror("unable to obtaining cur directory\n");
    return NULL;
  }
  std::string* cwd = new std::string(cwdPtr);

#ifdef _WIN32
  std::replace(cwd->begin(), cwd->end(), '\\', '/');
  cwd->insert(0, "file:/");
#else
  cwd->insert(0, "file:");
#endif

  return cwd;
}
