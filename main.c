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

#define	THREADCOUNT 4
#define THREADMAX 25

#define	OPTSTR "wst:"


/* Prints the program usage */
static void
usage()
{
	printf("Usage:\n");
	printf("dircmp.out [-s] [-w] [-tn] FIRST SECOND\n");
	printf("FIRST ... path (full or relative) to first directory\n");
	printf("SECOND ... path (full or relative) to second directory\n");
	printf("-w ... compares content of regular files as well\n");
	printf("-s ... synchronizes content of both files\n");
	printf("-t ... specifies how many threads should be used for"
		    "checking the file content and copying\n");
}

int
main(int argc, char **argv)
{
	int sync = 0;
	int with_content = 0;
	int thread_count = THREADCOUNT;
	int opt;

	int res1, res2, res;
	char *src, *dst;

	/* Loading options */

	while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
		switch (opt) {
			case 's':
				sync = 1;
				break;
			case 'w':
				with_content = 1;
				break;
			case 't':
				thread_count = strtol(optarg, NULL, 0);
				if (thread_count <= 0 || thread_count > THREADMAX) {
					fprintf(stderr, "Invalid value for thread count: %d, using default value %d\n", thread_count, THREADCOUNT);
					thread_count = THREADCOUNT;
				}
				break;
			case '?':
				fprintf(stderr, "Unknown option: %c\n", optopt);
				usage();
				return (1);
		}
	}

	/* Loading arguments */
	argv = argv + optind;

	if ((argc - optind) != 2) {
		fprintf(stderr, "Invalid number of arguments\n");
		usage();
		return (1);
	}

	src = argv[0];
	dst = argv[1];

	/* Performing the search */
	/* each search is one-directional */
	res1 = search(src, dst, with_content, 1, sync, thread_count);
	res2 = search(dst, src, 0, 0, sync, thread_count);

	/* Merging results of both searches */
	res = res1 || res2;

	/* Final report */
	if (res) {
		printf("Directories %s and %s have different content.\n",
			    src, dst);
		if (sync) {
			printf("Content of folders %s and %s was synchronized.\n",
				    dst, src);
		}
	}

	return (res);
}
