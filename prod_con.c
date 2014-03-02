#include "prod_con.h"
#include "pthread_wrapper.h"

void
init_queue(struct queue *q)
{
	q->start_pos = 0;
	q->queue_size = 0;
	mutex_init(&(q->m));
	cond_init(&(q->c_prod));
	cond_init(&(q->c_cons));
}

void
delete_queue(struct queue *q)
{
	mutex_destroy(&(q->m));
	cond_destroy(&(q->c_prod));
	cond_destroy(&(q->c_cons));
}

void
produce(struct queue *q, void *item)
{
	/* Lock */
	lock(&(q->m));

	/* Queue is full */
	while (q->queue_size == QUEUE_MAX) {
		/* Conditional wait */
		thread_wait(&(q->c_prod), &(q->m));
	}
	
	/* Add the item to the queue */
	q->data[(q->start_pos + q->queue_size) % QUEUE_MAX] = item;
	q->queue_size++;
	
	thread_broadcast(&(q->c_cons));

	unlock(&(q->m));
}

void
*consume(struct queue *q)
{
	void *result;

	/* Lock */
	lock(&(q->m));

	/* Queue is empty */
	while (q->queue_size <= 0) {
		thread_wait(&(q->c_cons), &(q->m));
	}

	/* Fetch the item from the queue */
	result = q->data[q->start_pos % QUEUE_MAX];

	/* NULL pointer in the queue means termination, we keep it in there */
	if (result != NULL) {
		/* Change the start and size of the queue */
		q->start_pos = (q->start_pos + 1) % QUEUE_MAX;

		q->queue_size--;
		
		/* Wake up the producer, there is free space */
		thread_signal(&(q->c_prod));
	}

	unlock(&(q->m));

	return (result);
}
