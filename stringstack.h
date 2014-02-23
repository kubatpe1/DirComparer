#ifndef STRINGSTACK_H
#define STRINGSTACK_H

/* Structure for storing the stack data */
struct stack {
	char **data;
	int size;
	int top;
};

/* Initializes the stack */
void stack_init(struct stack *stack, int start_size);

/* Adds item to the top of the stack - reallocates if required */
void stack_push(struct stack *stack, char *item);

/* Returns the item from the top of the stack (and removes it) */
char *stack_pop(struct stack *stack);

/* Deallocates the stack - must not be used afterwards! */
void stack_delete(struct stack *stack);

#endif /* STRINGSTACK_H */