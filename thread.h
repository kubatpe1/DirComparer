#ifndef THREAD_H
#define THREAD_H

#include "prod_con.h"

void *start(void *arg);

/* Compares content of two files, byte by byte */
/* Returns 1 if equal, 0 if different, or -1 in case of error */
int match_content(char *first, char *second);

/* Copies the source file to destination, overwrites if already exists */
void copy(char *source, char *destination);

#endif /* THREAD_H */
