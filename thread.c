#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>

#include "thread.h"

#define BUFSIZE 25

void *start(void *arg)
{
	return (NULL);
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
	
	if ((fd2 = open(destination, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
		warn("Error opening a file");
		close(fd1);
		return (-1);
	}
	
	/* Reading a file */
	while ((rd = read(fd1, buf, BUFSIZE)) > 0) {
		
		bufptr = buf;
		
		/* Writing to a file */
		while (rd > 0) {
			if ((written = write(fd2, buf, BUFSIZE)) < 0) {
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
	
	return (0);
}