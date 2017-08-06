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
