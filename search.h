#ifndef SEARCH_H
#define SEARCH_H

#include <pthread.h>

#include "prod_con.h"

/* Starting size of the string stack for crawl function */
#define STACKSIZE 25

/* Mask for directory creation */
#define DIRMASK 0777

/* Structure defining search context and parameters */
struct search_context {
	int result;
	struct queue q;
	char *source;
	char *target;
	int with_content;
	int sync;
	pthread_mutex_t output_lock;
};

/* Performs the search with specified parameters */
int search(char *src, char *dst, int with_content, int sync, int thread_num);

/* Builds path by concatenating strings, user has to free the memory himself */
char *build_path(char *rel_path, char *path_base, int include_slash);

#endif /* SEARCH_H */
