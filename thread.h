#ifndef THREAD_H
#define THREAD_H

#include "prod_con.h"

/* Entry function for the thread */
void *start(void *arg);

/* Performs file comparing and copying */
void compare(struct search_context *cont, char *file);

/* Compares content of two files, byte by byte */
/* Returns 1 if equal, 0 if different, or -1 in case of error */
int match_content(char *first, char *second);

/* Copies the source file to destination, overwrites if already exists */
int copy(char *source, char *destination);

#endif /* THREAD_H */
