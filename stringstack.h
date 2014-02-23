#ifndef STRINGSTACK_H
#define STRINGSTACK_H

struct stack {
	char **data;
	int size;
	int top;
};

void stack_init(struct stack *stack, int start_size);

void stack_push(struct stack *stack, char *item);

char *stack_pop(struct stack *stack);

void stack_delete(struct stack *stack);

#endif /* STRINGSTACK_H */