#include "prod_con.h"
#include "pthread_wrapper.h"

void
init_queue()
{
	start_pos = 0;
	queue_size = 0;
	mutex_init(&m);
	cond_init(&c_prod);
	cond_init(&c_cons);	
}

void
reset_queue()
{
    
}

void
delete_queue()
{
	mutex_destroy(&m);
	cond_destroy(&c_prod);
	cond_destroy(&c_cons);
}

void
produce(void *item)
{
	/* Lock */
	lock(&m);

	/* Queue is full */
	while (queue_size == QUEUE_MAX) {
		/* Conditional wait */
		wait(&c_prod, &m);
	}
	
	/* Add the item to the queue */
	queue[(start_pos + queue_size) % QUEUE_MAX] = item;
	queue_size++;
	
	signal(&c_cons);

	unlock(&m);
}

void
*consume()
{
	void *result;

	/* Lock */
	lock(&m);

	/* Queue is empty */
	while (queue_size <= 0) {
		wait(&c_cons, &m);
	}

	/* Fetch the item from the queue */
	result = queue[start_pos % QUEUE_MAX];

	/* NULL pointer in the queue means termination, we keep it in there */
	if (result != NULL) {
		/* Change the start and size of the queue */
		start_pos = (start_pos + 1) % QUEUE_MAX;

		queue_size--;
		
		/* Wake up the producer, there is free space */
		signal(&c_prod);
	}

	unlock(&m);

	return (result);
}
