#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "thread.h"
#include "search.h"
#include "stringstack.h"
#include "pthread_wrapper.h"
#include "prod_con.h"

/* Spawns all the consumer threads */
static void
spawn_threads(int n, pthread_t *buffer, struct search_context *context)
{
	int i, e;
	
	for (i = 0; i < n; i++) {
		if ((e = pthread_create(buffer + i,
								NULL, start, (void*)context)) != 0) {
			fprintf(stderr, "Error creating thread!\n");
			exit(1);
		}
	}
}

/* Builds path by concatenating strings, user has to free the memory himself */
char *
build_path(char *rel_path, char *path_base, int include_slash)
{
	int len = strlen(path_base) + strlen(rel_path) + (include_slash ? 2 : 1);
	
	char *result = calloc(len, sizeof (char));
	
	if (result == NULL) {
		err(1, "Error allocating memory");
	}
	
	strncpy(result, path_base, strlen(path_base));
	
	if (include_slash) {
		strncpy(result + strlen(path_base), "/", 1);
		strncpy(result + strlen(path_base) + 1, rel_path, strlen(rel_path));
	}
	else {
		strncpy(result + strlen(path_base), rel_path, strlen(rel_path));
	}
	
	return result;
}

/* Crawls the directory tree and adds all non-directory files to queue */
static void
crawl_directories(struct search_context *context)
{
	/* Stack for directories to crawl through */
	struct stack st;
	
	/* Directory and its entries */
	DIR *d;
	struct dirent *de;
	
	char *current, *path;
	
	/* Full directory paths */
	char *first, *second;
	
	char *empty = "";
	
	/* File stat structure */
	struct stat buf;
	
	/* Initialize the stack with an empty directory */
	stack_init(&st, STACKSIZE);
	stack_push(&st, empty);
	
	while ((current = stack_pop(&st)) != NULL) {
		
		/* Rebuilding the paths */
		first = build_path(current, context->source, 0);
		second = build_path(current, context->target, 0);
		
		/* Ensuring that both exist and are directories */
		if ((stat(first, &buf) == -1) || !S_ISDIR(buf.st_mode)) {
			/* File either doesn't exist, or is not a directory */
			printf("Directory %s doesn't exist!\n", first);
			context->result = 1;
			goto finish;
		}
		
		if ((stat(second, &buf) == -1) || (!S_ISDIR(buf.st_mode))) {
			/* Directory doesn't exist in the other tree */
			context->result = 1;
			pthread_lock_checked(&(context->output_lock));
			printf("Directory %s doesn't exist!\n", second);
			pthread_unlock_checked(&(context->output_lock));
			/* Creating the directory */
			if (context->sync) {
				if (mkdir(second, DIRMASK) == -1) {
					pthread_lock_checked(&(context->output_lock));
					printf("Directory %s can't be created!"
						   "\n", second);
					pthread_unlock_checked(&(context->output_lock));
					goto finish;
				}
				/* Directory succesfully created */
			}
			/* If we don't create the directory, we just skip it */
			else goto finish;
		}
		
		/* Directory can't be opened */
		if ((d = opendir(first)) == NULL) {
			pthread_lock_checked(&(context->output_lock));
			fprintf(stderr, "Error opening the directory: "
				    "%s\n", current);
			pthread_unlock_checked(&(context->output_lock));
			
			exit(1);
		}
		
		while ((de = readdir(d)) != NULL) {
			/* Excluding loopback hardlinks */
			if ((strcmp(de->d_name, ".") == 0) ||
				(strcmp(de->d_name, "..") == 0)) {
				continue;
			}
			
			/* Constructing new relative path */
			path = build_path(de->d_name, current, 1);
			
			/* Constructing absolute paths */
			free(first);
			free(second);
			first = build_path(path, context->source, 0);
			second = build_path(path, context->target, 0);
			
			/* Reading file stats */
			if (stat(first, &buf) == -1) {
				pthread_lock_checked(&(context->output_lock));
				fprintf(stderr, "Error reading the file "
					    "stats: %s\n", first);
				pthread_unlock_checked(&(context->output_lock));
				
				free(path);
				continue;
			}
			
			/* Directory - add to crawling stack */
			if (S_ISDIR(buf.st_mode)) {
				stack_push(&st, path);
				continue;
			}
			/* Other file - add to queue for threads to process */
			else {
				produce(&(context->q), path);
			}
		}
		
		if (closedir(d) == -1) {
			pthread_lock_checked(&(context->output_lock));
			fprintf(stderr, "Error closing the directory!\n");
			pthread_unlock_checked(&(context->output_lock));
			
			exit(1);
		}
		
		/* Final deallocation block */
	finish:
		free(first);
		free(second);
		if (current != empty) {
			free(current);
		}
	}
	
	stack_delete(&st);
}

int
search(char *src, char *dst, int with_content, int sync, int thread_num)
{
	/* Search context structure accessed by all the threads */
	struct search_context context;

	/* Array of IDs of all spawned threads */
	pthread_t *thread_ids;

	int i;

	/* Result - 0 = trees are similar, 1 = trees are different */
	context.result = 0;
	context.source = src;
	context.target = dst;
	context.with_content = with_content;
	context.sync = sync;
	init_queue(&(context.q));
	pthread_mutex_init_checked(&(context.output_lock));

	thread_ids = malloc(sizeof (pthread_t) * thread_num);

	spawn_threads(thread_num, thread_ids, &context);

	/* Starting file tree crawling */
	crawl_directories(&context);

	/* Producing NULL value to stop all the threads */
	produce(&(context.q), NULL);

	/* Waiting for all threads to end */
	for (i = 0; i < thread_num; i++) {
		pthread_join(thread_ids[i], NULL);
	}

	/* Cleanup */
	pthread_mutex_destroy_checked(&(context.output_lock));

	return (context.result);
}
