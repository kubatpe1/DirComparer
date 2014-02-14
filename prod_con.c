#include "prod_con.h"

void
init_queue()
{
	int e;
	start_pos = 0;
	queue_size = 0;
	if ((e = pthread_mutex_init(&m, NULL)) != 0)
		errx(1, "Error initializing mutex: %s", strerror(e));
	
	if ((e = pthread_cond_init(&c_prod, NULL)) != 0)
		errx(1, "Error initializing cond: %s", strerror(e));
	
	if ((e = pthread_cond_init(&c_cons, NULL)) != 0)
		errx(1, "Error initializing cond: %s", strerror(e));
}

void
delete_queue()
{

}

int 
produce(void *item)
{
	int e;
	/* Lock */
	if ((e = pthread_mutex_lock(&m)) != 0)
		errx(1, "Error locking mutex: %s", strerror(e));
	/* Queue is full */
	while (queue_size == QUEUE_MAX) {
		/* Conditional wait */
		if ((e = pthread_cond_wait(&c_prod, &m)) != 0)
			errx(1, "Error waiting: %s", strerror(e));
	}


		
}
