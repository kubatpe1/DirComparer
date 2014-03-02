#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <sys/stat.h>

#include "search.h"
#include "thread.h"
#include "pthread_wrapper.h"

#define BUFSIZE 25

void *start(void *arg)
{
	struct search_context *context = (struct search_context *)arg;
	
	/* Relative path to a file - gets from queue from main thread */
	char *file;
	
	while ((file = consume(&context->q)) != NULL) {
		compare_files(context, file);
	}
	
	return (NULL);
}

void
compare_files(struct search_context *context, char *file)
{
	/* Complete paths to both files */
	char *first;
	char *second;
		
	/* 0 = files are similar, 1 = files are different or the other one doesn't exist */
	int different = 0;
	
	/* 0 = file won't be copied, 1 = file will be copied (replacing original one) 2 = file will be copied the other way */
	int sync = 0;
	
	/* Structures for information about both files */
	struct stat first_stat;
	struct stat second_stat;
	
	int res;

	
	/* Constructing paths */
	build_paths(file, context->source, context->target, &first, &second);
	
	/* Comparing files */
	
	/* 1. Existence */
	if (stat(first, &first_stat) == -1) {
		fprintf(stderr, "Error, can't read file stats: %s\n", first);
		exit(1);
	}
	if (stat(second, &second_stat) == -1) {
		different = 1;
		sync = 1;
		lock(&(context->output_lock));
		printf("File %s: doesn't exist!\n", second);
		unlock(&(context->output_lock));
		goto finish;
	}
	
	if (context->with_content) {
		
		/* Change date check */
		if (first_stat.st_mtime > second_stat.st_mtime) {
			sync = 1;
		}
		else {
			sync = 2;
		}
		
		/* 2. Size */
		if (first_stat.st_size != second_stat.st_size) {
			different = 1;
			lock(&(context->output_lock));
			printf("Files have different size:\n");
			printf("%s - %llu\n", first, (unsigned long long)first_stat.st_size);
			printf("%s - %llu\n", second, (unsigned long long)second_stat.st_size);
			unlock(&(context->output_lock));
			goto finish;
		}
		
		/* 3. Type */
		if ((first_stat.st_mode & S_IFMT) != (second_stat.st_mode & S_IFMT)) {
			different = 1;
			lock(&(context->output_lock));
			printf("Files %s and %s have different type.\n", first, second);
			unlock(&(context->output_lock));
			goto finish;
		}
		
		/* 4. Content */
		if (context->with_content && S_ISREG(first_stat.st_mode)) {
			res = match_content(first, second);
			if (res == -1) {
				printf("Files %s and %s can't be compared.\n", first, second);
				sync = 0;
			}
			if (!res) {
				different = 1;
				lock(&(context->output_lock));
				printf("Files %s and %s have different content!\n", first, second);
				unlock(&(context->output_lock));
				goto finish;
			}
		}
			
	}
	
finish:
	/* Optional - copy */
	if (different && context->sync) {
		if (sync == 1) {
			if (copy(first, second) == -1) {
				lock(&(context->output_lock));
				printf("File %s can't be copied to destination %s.\n", first, second);
				unlock(&(context->output_lock));
			}
		}
		else if (sync == 2) {
			if (copy(second, first) == -1) {
				lock(&(context->output_lock));
				printf("File %s can't be copied to destination %s.\n", second, first);
				unlock(&(context->output_lock));
			}
		}
	}
	
	/* Reporting difference */
	if (different) context->result = 1;
	
	/* Freeing the memory (including consumed string from queue) */
	free(first);
	free(second);
	free(file);
}

int
match_content(char *first, char *second)
{
	/* Buffers for storing content */
	char buf1[BUFSIZE], buf2[BUFSIZE];
	/* File descriptors */
	int fd1, fd2;
	int read1, read2;
	int ret_val = 1;
	int i;
	
	if ((fd1 = open(first, O_RDONLY)) == -1) {
		warn("Error opening a file");
		return (-1);
	}
	
	if ((fd2 = open(second, O_RDONLY)) == -1) {
		warn("Error opening a file");
		close(fd1);
		return (-1);
	}
		
	while (1) {
		read1 = read(fd1, buf1, BUFSIZE);
		read2 = read(fd2, buf2, BUFSIZE);
		
		/* Error reading */
		if (read1 == -1 || read2 == -1) {
			warn("Error reading a file");
			close(fd1);
			close(fd2);
			return (-1);
		}
		
		/* Files have different lengths */
		if (read1 != read2) {
			ret_val = 0;
			break;
		}
		
		/* Reached end of both files */
		if (read1 == 0) {
			break;
		}
		
		/* Comparing actual bytes */
		for (i = 0; i < read1; i++) {
			if (buf1[i] != buf2[i]) {
				ret_val = 0;
				break;
			}
		}
		
		/* If we found a difference, we can end */
		if (ret_val == 0) {
			break;
		}
	}
	
	close(fd1);
	close(fd2);
	
	return (ret_val);
}

int
copy(char *source, char *destination)
{
	/* Buffer for storing content */
	char buf[BUFSIZE];
	char *bufptr;
	/* File descriptors */
	int fd1, fd2;
	int rd, written;
	
	if ((fd1 = open(source, O_RDONLY)) == -1) {
		warn("Error opening a file");
		return (-1);
	}
	
	if ((fd2 = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
		warn("Error opening a file");
		close(fd1);
		return (-1);
	}
	
	/* Reading a file */
	while ((rd = read(fd1, buf, BUFSIZE)) > 0) {
		
		bufptr = buf;
		
		/* Writing to a file */
		while (rd > 0) {
			if ((written = write(fd2, buf, rd)) < 0) {
				warn("Error writing to a file");
				close(fd1);
				close(fd2);
				return (-1);
			}
			rd -= written;
			bufptr += written;
		}
	}
	
	if (rd < 0) {
		warn("Error writing to a file");
		close(fd1);
		close(fd2);
		return (-1);
	}
	
	close(fd1);
	close(fd2);
	
	return (0);
}