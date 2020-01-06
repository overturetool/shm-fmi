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
#include "fmi2Functions.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char* fmi2StatusToString(fmi2Status status) {
  switch (status) {
    case fmi2OK:
      return "ok";
    case fmi2Warning:
      return "warning";
    case fmi2Discard:
      return "discard";
    case fmi2Error:
      return "error";
    case fmi2Fatal:
      return "fatal";
#ifdef FMI_COSIMULATION
    case fmi2Pending:
      return "fmi2Pending";
#endif
    default:
      return "?";
  }
}

#define MAX_MSG_SIZE 1000
void fmuLogger(fmi2ComponentEnvironment componentEnvironment,
               fmi2String instanceName, fmi2Status status, fmi2String category,
               fmi2String message, ...) {
  char msg[MAX_MSG_SIZE];
  char* copy;
  va_list argp;

  // replace C format strings
  va_start(argp, message);
  vsprintf(msg, message, argp);
  va_end(argp);

  // replace e.g. ## and #r12#
  copy = strdup(msg);
  // FIXME replaceRefsInMessage(copy, msg, MAX_MSG_SIZE, &fmu);
  free(copy);

  // print the final message
  if (!instanceName) instanceName = "?";
  if (!category) category = "?";
  printf("%s %s (%s): %s\n", fmi2StatusToString(status), instanceName, category,
         msg);
}
