#ifndef SEARCH_H
#define SEARCH_H

#include <pthread.h>

#include "prod_con.h"

/* Global lock for console output */
pthread_mutex_t console_lock = PTHREAD_MUTEX_INITIALIZER;

/* Structure defining search context and parameters */
struct search_context {
	struct queue q;
	char *source;
	char *target;
	int with_content;
	int sync;
};

/* Performs the search with specified parameters */
int search(char *src, char *dst, int with_content, int sync);

#endif /* SEARCH_H */
