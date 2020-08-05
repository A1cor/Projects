/*This module implements a stack.*/
/*Stack struct*/

#ifndef STACK_H_
#define STACK_H_


#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdlib.h>

#if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
typedef int bool;
#define true 1
#define false 0
#define __bool_true_false_are_defined
#endif

typedef struct Stack Stack;
typedef struct StackNode {
	int i;
	int j;
	int *arr_of_options;
	struct StackNode *next;
} StackNode;

/*creates an empty stack*/
Stack *stackCreate();

/*frees the stack and all the memory related to it*/
void stackDestroy(Stack *stack);

/*returns stack size*/
int stackSize(Stack *stack);

/*returns top element*/
StackNode *stackTop(Stack *stack);

/*pushes an element to the stack*/
bool stackPush(Stack *stack, int i, int j, int *arr_of_options);

/*pops an element from the stack*/
StackNode *stackPop(Stack *stack);

#endif
#endif /* STACK_H_ */
