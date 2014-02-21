#ifndef PTHREAD_WRAPPER
#define PTHREAD_WRAPPER

#include <pthread.h>

/* Wrapper functions for error detection */
void cond_init(pthread_cond_t *c);
void cond_destroy(pthread_cond_t *c);
void mutex_init(pthread_mutex_t *m);
void mutex_destroy(pthread_mutex_t *m);
void lock(pthread_mutex_t *m);
void unlock(pthread_mutex_t *m);
void wait(pthread_cond_t *c, pthread_mutex_t *m);
void signal(pthread_cond_t *c);

#endif
