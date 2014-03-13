#include <pthread.h>
#include <err.h>
#include <string.h>

#include "pthread_wrapper.h"

void
cond_init(pthread_cond_t *c)
{
	int e;

	if ((e = pthread_cond_init(c, NULL)) != 0)
		errx(1, "Error initializing cond: %s", strerror(e));
}


void
cond_destroy(pthread_cond_t *c)
{

	int e;
	if ((e = pthread_cond_destroy(c)) != 0)
		errx(1, "Error destroying cond: %s", strerror(e));
}


void
mutex_init(pthread_mutex_t *m)
{
	int e;

	if ((e = pthread_mutex_init(m, NULL)) != 0)
		errx(1, "Error initializing mutex: %s", strerror(e));
}


void
mutex_destroy(pthread_mutex_t *m)
{
	int e;
	if ((e = pthread_mutex_destroy(m)) != 0)
		errx(1, "Error destroying mutex: %s", strerror(e));
}

void
lock(pthread_mutex_t *m)
{
	int e;

	if ((e = pthread_mutex_lock(m)) != 0)
		errx(1, "Error locking mutex: %s", strerror(e));
}

void
unlock(pthread_mutex_t *m)
{
	int e;

	if ((e = pthread_mutex_unlock(m)) != 0)
		errx(1, "Error unlocking mutex: %s", strerror(e));
}

void
thread_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
	int e;

	if ((e = pthread_cond_wait(c, m)) != 0)
		errx(1, "Error waiting: %s", strerror(e));
}

void
thread_signal(pthread_cond_t *c)
{
	int e;

	if ((e = pthread_cond_signal(c)) != 0)
		errx(1, "Error signaling: %s", strerror(e));
}

void
thread_broadcast(pthread_cond_t *c)
{
	int e;

	if ((e = pthread_cond_broadcast(c)) != 0)
		errx(1, "Error broadcasting: %s", strerror(e));
}
