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
	struct queue q;
	char *source;
	char *target;
	int with_content;
	int sync;
	pthread_mutex_t output_lock;
};

/* Performs the search with specified parameters */
int search(char *src, char *dst, int with_content, int sync, int thread_num);

/* Spawns all the consumer threads */
void spawn_threads(int n, pthread_t *buffer, struct search_context *context);

/* Crawls the directory tree and adds all non-directory files to queue */
void crawl_directories(struct search_context *context);

/* Builds two paths by concatenating strings, user has to free the memory himself */
void build_paths(char *rel_path, char *first, char *second, char **first_res, char **second_res);


#endif /* SEARCH_H */
