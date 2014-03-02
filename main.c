#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>
#include <pthread.h>

#include "search.h"
#include "stringstack.h"
#include "thread.h"
#include "prod_con.h"

#define STACKSIZE 25
#define THREADCOUNT 4


/* Test section!!! TODO: DELETE THIS */

/*
void
test()
{
	int i, j;
	char *data;
	for (j = 0; j < 100; j++) {
		data = calloc(sizeof (char) * 15, 0);
		sprintf(data, "%d", j);
		produce(data);
	}
	produce(NULL);
}
 */

/* End of test section!!! TODO: DELETE THIS */

/*
void
search(char *start, struct queue *q)
{
	struct stack st;
	DIR *d;
	struct dirent *de;
	char *current, *path;
	int len;
	struct stat buf;
	int written, k;

	stack_init(&st, STACKSIZE);

	stack_push(&st, start);

	while ((current = stack_pop(&st)) != NULL) {

		if ((d = opendir(current)) == NULL) {
			fprintf(stderr, "Error opening the directory: %s\n", current);
			continue;
		}
		
		while ((de = readdir(d)) != NULL) {
			if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
				continue;
			}

			len = strlen(current);
			path = malloc((len + strlen(de->d_name) + 2) * sizeof (char));
			strcpy(path, current);
			strcpy(path + len, "/");
			strcpy(path + len + 1, de->d_name);

			printf("Reading file %s\n", path);
			
			if (stat(path, &buf) == -1) {
				fprintf(stderr, "Error reading the file stats: %s\n", path);
				free(path);
				continue;
			}

			if (S_ISDIR(buf.st_mode)) {
				stack_push(&st, path);
				continue;
			}
			
			if (S_ISREG(buf.st_mode)) {
				produce(q, path);
			}
		}

		if (closedir(d) == -1) {
			fprintf(stderr, "Error closing the directory!\n");
			exit(1);
		}
		
		if (current != start) {
			free(current);
		}
	}

	stack_delete(&st);
	
	sleep(5);
}
 */

/*
void
parent(int in)
{
	off_t buf;
	uint64_t sum, oldsum;
	int n, rd;

	sum = 0;
	
	rd = 0;
	while ((n = read(in, (&buf + rd), (sizeof (off_t) - rd))) > 0) {
		printf("Read %d bytes\n", n);
		rd += n;

		rd = rd % sizeof (off_t);
		if (rd == 0) {
			oldsum = sum;
			sum += buf;
			if (sum < oldsum) {
				fprintf(stderr, "Overflow!\n");
				exit(1);
			}
		}
	}

	if (n < 0) {
		fprintf(stderr, "Error reading from pipe\n");
		exit(1);
	}

	printf("Total size: %" PRIu64 "\n", sum);
}
*/

/*
void
*start(void *arg)
{
	char *msg;
	int n = (int)arg;
	
	printf("Thread %d spawned!", n);

	while ((msg = consume(&q)) != NULL) {
		printf("%d : %s\n", n, msg);
		sleep(1);
	}

	return NULL;
}
 */

/*
void 
spawn_threads(int n, pthread_t *buffer)
{
	int i, e;
	
	for (i = 0; i < n; i++) {
		if ((e = pthread_create(buffer + i, NULL, start, (void*)i)) != 0) {
			fprintf(stderr, "Error creating thread!\n");
			exit(1);
		}
	}
}
 */

int 
main(int argc, char **argv)
{
	/*
	int res1 = match_content("main.c", "main2.x");
	int res2 = match_content("main.c", "search.c");
	copy("main.c", "main3.z");
	
	printf("%d, %d\n", res1, res2);
	
	return (0);
	
	int n = 5;
	pthread_t *buffer = malloc(sizeof (pthread_t) * n);
	init_queue(&q);
	spawn_threads(n, buffer);
	search("../..", &q);
	return (0);
	 */
	
	int res1, res2, res;
	
	int sync = 0;
	
	char *src = "F6";
	char *dst = "F7";
	
	res1 = search(src, dst, 1, sync, 8);
	res2 = search(dst, src, 0, sync, 8);
	
	res = res1 || res2;
	
	/* Final report */
	if (!res) {
		printf("Directories %s and %s have similar content.\n", src, dst);
	}
	else {
		printf("Directories %s and %s have different content.\n", src, dst);
		if (sync) {
			printf("Content of %s was synchronized with content of %s.\n", dst, src);
		}
	}
	
	return (res);

	/*
	int pid, status, threadn;
	pthread_t *threads;


	if (argc != 2) {
		fprintf(stderr, "Requires exactly one argument!\n");
		return (1);
	}
	
	* Queue initialization *
	init_queue();

	threadn = THREADCOUNT;
	threads = malloc(sizeof (pthread_t) * threadn);
	
	spawn_threads(threadn, threads);
	
	* USEFUL CODE HERE! *

	
	
	free(threads);
	return (0);

	*/
}
