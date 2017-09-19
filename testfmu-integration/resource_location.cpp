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
