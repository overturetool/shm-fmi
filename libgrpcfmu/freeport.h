/*
 * freeport.h
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#ifndef FREEPORT_H_
#define FREEPORT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

unsigned short int getFreePort();

#ifdef __cplusplus
}
#endif
#endif /* FREEPORT_H_ */
