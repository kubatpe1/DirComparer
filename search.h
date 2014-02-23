#ifndef SEARCH_H
#define SEARCH_H

#include "prod_con.h"

/* Structure defining search context and parameters */
struct search_context {
	struct queue q;
	char *source;
	char *target;
	int with_content;
	int sync;
};

/* Performs the search with specified parameters */
int search(char *src, char *dst, int with_content, int sync);

#endif /* SEARCH_H */
