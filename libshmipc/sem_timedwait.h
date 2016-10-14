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
