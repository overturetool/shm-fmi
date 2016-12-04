/*
 * fmu-loader.h
 *
 *  Created on: Aug 22, 2015
 *      Author: parallels
 */

#ifndef FMU_LOADER_H_
#define FMU_LOADER_H_

#include "fmi2.h"

int loadDll(const char* dllPath, FMU* fmu, HMODULE* h);

#endif /* FMU_LOADER_H_ */
