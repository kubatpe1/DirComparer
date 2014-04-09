#include "prod_con.h"
#include "pthread_wrapper.h"

void
init_queue(struct queue *q)
{
	q->start_pos = 0;
	q->queue_size = 0;
	pthread_mutex_init_checked(&(q->m));
	pthread_cond_init_checked(&(q->c_prod));
	pthread_cond_init_checked(&(q->c_cons));
}

void
delete_queue(struct queue *q)
{
	pthread_mutex_destroy_checked(&(q->m));
	pthread_cond_destroy_checked(&(q->c_prod));
	pthread_cond_destroy_checked(&(q->c_cons));
}

void
produce(struct queue *q, void *item)
{
	/* Lock */
	pthread_lock_checked(&(q->m));

	/* Queue is full */
	while (q->queue_size == QUEUE_MAX) {
		/* Conditional wait */
		pthread_wait_checked(&(q->c_prod), &(q->m));
	}

	/* Add the item to the queue */
	q->data[(q->start_pos + q->queue_size) % QUEUE_MAX] = item;
	q->queue_size++;

	pthread_broadcast_checked(&(q->c_cons));

	pthread_unlock_checked(&(q->m));
}

void
*consume(struct queue *q)
{
	void *result;

	/* Lock */
	pthread_lock_checked(&(q->m));

	/* Queue is empty */
	while (q->queue_size <= 0) {
		pthread_wait_checked(&(q->c_cons), &(q->m));
	}

	/* Fetch the item from the queue */
	result = q->data[q->start_pos % QUEUE_MAX];

	/* NULL pointer in the queue means termination, we keep it in there */
	if (result != NULL) {
		/* Change the start and size of the queue */
		q->start_pos = (q->start_pos + 1) % QUEUE_MAX;

		q->queue_size--;

		/* Wake up the producer, there is free space */
		pthread_signal_checked(&(q->c_prod));
	}

	pthread_unlock_checked(&(q->m));

	return (result);
}
