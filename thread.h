#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include "prod_con.h"
#include "search.h"

/* Entry function for the thread */
void *start(void *arg);

#endif /* THREAD_H */
