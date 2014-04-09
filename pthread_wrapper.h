#ifndef PTHREAD_WRAPPER
#define	PTHREAD_WRAPPER

#include <pthread.h>

/* Wrapper functions for error detection */
void pthread_cond_init_checked(pthread_cond_t *c);
void pthread_cond_destroy_checked(pthread_cond_t *c);
void pthread_mutex_init_checked(pthread_mutex_t *m);
void pthread_mutex_destroy_checked(pthread_mutex_t *m);
void pthread_lock_checked(pthread_mutex_t *m);
void pthread_unlock_checked(pthread_mutex_t *m);
void pthread_wait_checked(pthread_cond_t *c, pthread_mutex_t *m);
void pthread_signal_checked(pthread_cond_t *c);
void pthread_broadcast_checked(pthread_cond_t *c);

#endif
