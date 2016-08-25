#ifdef __APPLE__

#include "sem_timedwait.h"
#include <mach/clock.h>
#include <mach/mach.h>

int clock_gettime(clockid_t clk_id, struct timespec *ts)
{
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;

	return 0;
}

static useconds_t timespec_to_milisec(const struct timespec* spec)
{
	useconds_t ms; // Milliseconds
	time_t s;  // Seconds

	s = spec->tv_sec;
	ms = round(spec->tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

	return (s * 1000) + ms;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
	while (sem_trywait(sem) == -1 && errno == EAGAIN)
	{
		struct timespec ts;
		clock_gettime(0, &ts);

		useconds_t now = timespec_to_milisec(&ts);
		useconds_t delay = timespec_to_milisec(abs_timeout);

		if (delay > now)
		{
			useconds_t ms = delay - now;
			usleep(ms);
		} else
		{
			//Did not manage to accuire the semaphor so return
			return -1;
		}
	}
	return 0;
}

#endif
