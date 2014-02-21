#include <err.h>
#include <pthread.h>

#define QUEUE_MAX 25

void *queue[QUEUE_MAX];
int start_pos;
int queue_size;
pthread_mutex_t m;
pthread_cond_t c_prod;
pthread_cond_t c_cons;

/* Initializes the queue, mutex and cond. variable */
void init_queue();
/* Cleans up the queue */
void delete_queue();
/* Adds one item into the queue (and wakes sleeping consumers) */
void produce(void *item);
/* Takes one item from the queue (and wakes sleeping producers */
void *consume();
