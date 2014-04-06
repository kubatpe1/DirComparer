#include <stdio.h>
#include <stdlib.h>

#include "stringstack.h"

#define	STEP 15

void
stack_init(struct stack *stack, int start_size)
{
	if (stack == NULL) {
		fprintf(stderr, "Stack is NULL!\n");
		exit(1);
	}

	if (start_size <= 0) {
		fprintf(stderr, "Stack size must be positive\n");
		exit(1);
	}

	stack->size = start_size;
	stack->top = -1;
	stack->data = malloc((stack->size) * sizeof (char *));
	if (stack->data == NULL) {
		fprintf(stderr, "Error allocating the memory!\n");
		exit(1);
	}
}

void
stack_push(struct stack *stack, char *item)
{
	if (stack == NULL) {
		fprintf(stderr, "Stack is NULL!\n");
		exit(1);
	}

	if (stack->data == NULL) {
		fprintf(stderr, "Use of uninitialized stack!\n");
		exit(1);
	}
	/* Need to reallocate memory */
	if (stack->top >= stack->size - 1) {
		stack->size += STEP;
		printf("Realloc with %d\n", stack->size);
		stack->data = realloc((void *)(stack->data), stack->size * sizeof (char *));
		if (stack->data == NULL) {
			fprintf(stderr, "Error allocating the memory!\n");
			exit(1);
		}

	}

	stack->data[++(stack->top)] = item;
}

char
*stack_pop(struct stack *stack)
{
	if (stack == NULL) {
		fprintf(stderr, "Stack is NULL!\n");
		exit(1);
	}

	if (stack->top < 0) {
		return (NULL);
	} else {
		return (stack->data[(stack->top)--]);
	}
}

void
stack_delete(struct stack *stack)
{
	if (stack == NULL) {
		fprintf(stderr, "Stack is NULL!\n");
		exit(1);
	}

	if (stack->data == NULL) {
		return;
	}

	free(stack->data);
	stack->data = NULL;
	stack->size = 0;
	stack->top = -1;
}