/*This module implements a stack.*/


#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "solve.h"


struct Stack {
	int count;
	StackNode *top;
};

/*creates an empty stack*/
Stack *stackCreate()
{
	Stack *stack = (Stack *) malloc(sizeof(stack));
	if(stack==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	stack->count = 0;
	stack->top = 0;

	return stack;
}
/*helper function for stackDestroy*/
void stackClean(Stack *stack){

	StackNode *node;
	while(stackSize(stack)>0){
		node = stackPop(stack);
		free(node->arr_of_options);
		free(node);
	}
}
/*frees the stack and all the memory related to it*/
void stackDestroy(Stack *stack)
{
	stackClean(stack);
	free(stack);
}

/*returns stack size*/
int stackSize(Stack *stack)
{
	return stack->count;
}

/*returns top element*/
StackNode *stackTop(Stack *stack)
{
	return stack->top;
}

/*pushes an element to the stack*/
bool stackPush(Stack *stack, int i, int j, int *arr_of_options)
{
	StackNode *newNode = (StackNode *) malloc(sizeof *newNode);
	if(newNode==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	newNode->arr_of_options=arr_of_options;
	newNode->i=i;
	newNode->j=j;
	newNode->next = stack->top;
	stack->top = newNode;

	stack->count += 1;
	return true;
}

/*pops an element from the stack*/
StackNode *stackPop(Stack *stack)
{
	StackNode *oldTop;
	oldTop = stack->top;
	stack->top = oldTop->next;
	stack->count -= 1;
	return oldTop;
}

