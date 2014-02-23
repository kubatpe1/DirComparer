#ifndef PROD_CON_H
#define PROD_CON_H

#include <err.h>
#include <pthread.h>

#define QUEUE_MAX 25

/* Structure for storing the producer-consumer queue data and locks */
/* Size fixed by QUEUE_MAX constant */
struct queue {
	void *data[QUEUE_MAX];
	int start_pos;
	int queue_size;
	pthread_mutex_t m;
	pthread_cond_t c_prod;
	pthread_cond_t c_cons;
};

/* Initializes the queue, mutex and cond. variable */
void init_queue(struct queue *q);
/* Cleans up the queue */
void delete_queue(struct queue *q);
/* Adds one item into the queue (and wakes sleeping consumers) */
void produce(struct queue *q, void *item);
/* Takes one item from the queue (and wakes sleeping producers */
void *consume(struct queue *q);

#endif /* PROD_CON_H */