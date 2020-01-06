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
#ifndef __OSX_SEM_TIMEDWAIT_H_
#define __OSX_SEM_TIMEDWAIT_H_

#ifdef __APPLE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <math.h>

typedef int clockid_t;
static int CLOCK_REALTIME = 0;
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int clock_gettime(clockid_t clk_id, struct timespec *tp);
#endif
#endif
