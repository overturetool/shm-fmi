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
#ifdef __APPLE__

#include "sem_timedwait.h"
#include <mach/clock.h>
#include <mach/mach.h>

int clock_gettime(clockid_t clk_id, struct timespec *ts) {
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;

  return 0;
}

static useconds_t timespec_to_milisec(const struct timespec *spec) {
  useconds_t ms;  // Milliseconds
  time_t s;       // Seconds

  s = spec->tv_sec;
  ms = round(spec->tv_nsec / 1.0e6);  // Convert nanoseconds to milliseconds

  return (s * 1000) + ms;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
  while (sem_trywait(sem) == -1 && errno == EAGAIN) {
 struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    useconds_t now = timespec_to_milisec(&ts);
    useconds_t delay = timespec_to_milisec(abs_timeout);

    if (delay > now) {
      useconds_t ms = delay - now;
      usleep(ms);
    } else {
      // Did not manage to accuire the semaphor so return
      return -1;
    }
  }
  return 0;
}

#endif
