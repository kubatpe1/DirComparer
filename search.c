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

int
search(char *src, char *dst, int with_content, int sync, int thread_num)
{
	/* Search context structure accessed by all the threads */
	struct search_context context;
	
	/* Buffer for all the threads */
	pthread_t *buffer;
	
	int i;
	
	/* Result - 0 = trees are similar, 1 = trees are different */
	context.result = 0;
	context.source = src;
	context.target = dst;
	context.with_content = with_content;
	context.sync = sync;
	init_queue(&(context.q));
	mutex_init(&(context.output_lock));
	
	buffer = malloc(sizeof (pthread_t) * thread_num);
	
	spawn_threads(thread_num, buffer, &context);
	
	/* Starting file tree crawling */
	crawl_directories(&context);
	
	/* Producing NULL value to stop all the threads */
	produce(&(context.q), NULL);
	
	/* Waiting for all threads to end */
	for (i = 0; i < thread_num; i++) {
		pthread_join(buffer[i], NULL);
	}
	
	/* Cleanup */
	mutex_destroy(&(context.output_lock));
	
	return (context.result);
}

void
spawn_threads(int n, pthread_t *buffer, struct search_context *context)
{
	int i, e;
	
	for (i = 0; i < n; i++) {
		if ((e = pthread_create(buffer + i, NULL, start, (void*)context)) != 0) {
			fprintf(stderr, "Error creating thread!\n");
			exit(1);
		}
	}
}

void
crawl_directories(struct search_context *context)
{
	/* Stack for directories to crawl through */
	struct stack st;
	
	/* Directory and its entries */
	DIR *d;
	struct dirent *de;
	
	char *current, *path;
	int len;
	
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
		build_paths(current, context->source, context->target, &first, &second);
		
		/* Ensuring that both exist and are directories */
		if ((stat(first, &buf) == -1) || !S_ISDIR(buf.st_mode)) {
			lock(&(context->output_lock));
			fprintf(stderr, "Error, direcotry %s doesn't exist!\n", first);
			unlock(&(context->output_lock));

			exit(1);
		}
		
		if ((stat(second, &buf) == -1) || (!S_ISDIR(buf.st_mode))) {
			/* Directory doesn't exist in the other tree */
			context->result = 1;
			lock(&(context->output_lock));
			printf("Directory %s doesn't exist!\n", second);
			unlock(&(context->output_lock));
			/* Creating the directory */
			if (context->sync) {
				if (mkdir(second, DIRMASK) == -1) {
					lock(&(context->output_lock));
					printf("Directory %s can't be created!\n", second);
					unlock(&(context->output_lock));
					goto finish;
				}
				/* Directory succesfully created */
			}
			/* If we don't create the directory, we just skip it */
			else goto finish;
		}
		
		/* Directory can't be opened */
		if ((d = opendir(first)) == NULL) {
			lock(&(context->output_lock));
			fprintf(stderr, "Error opening the directory: %s\n", current);
			unlock(&(context->output_lock));

			exit(1);
		}
		
		while ((de = readdir(d)) != NULL) {
			/* Excluding loopback hardlinks */
			if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
				continue;
			}
			
			/* Constructing new relative path */
			len = strlen(current);
			path = malloc((len + strlen(de->d_name) + 2) * sizeof (char));
			strcpy(path, current);
			strcpy(path + len, "/");
			strcpy(path + len + 1, de->d_name);
			
			/* Constructing absolute paths */
			free(first);
			free(second);
			build_paths(path, context->source, context->target, &first, &second);
						
			/* Reading file stats */
			if (stat(first, &buf) == -1) {
				lock(&(context->output_lock));
				fprintf(stderr, "Error reading the file stats: %s\n", first);
				unlock(&(context->output_lock));

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
			lock(&(context->output_lock));
			fprintf(stderr, "Error closing the directory!\n");
			unlock(&(context->output_lock));

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

void
build_paths(char *rel_path, char *first, char *second, char **first_res, char **second_res)
{
	int first_len, second_len;
	
	/* Constructing paths */
	first_len = strlen(first) + strlen(rel_path) + 1;
	second_len = strlen(second) + strlen(rel_path) + 1;
	
	(*first_res) = calloc(first_len * sizeof(char), 0);
	(*second_res) = calloc(second_len * sizeof(char), 0);
	
	if (first == NULL || second == NULL) {
		err(1, "Error allocating memory");
	}
	
	strncpy((*first_res), first, strlen(first));
	strncpy((*first_res) + strlen(first), rel_path, strlen(rel_path));
	
	strncpy((*second_res), second, strlen(second));
	strncpy((*second_res) + strlen(second), rel_path, strlen(rel_path));
}


