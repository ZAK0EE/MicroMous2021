#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <inttypes.h>

#define MAX_ELEMENTS 16*16*2
typedef struct Stack
{
    uint16_t top;
    uint8_t entry[MAX_ELEMENTS];
}
Stack;

//Initializes the stack.
void CreateStack(Stack *ps);

//Pushs new element to the stack.
//Pre:Stack is not empty.
int8_t Push(uint8_t value, Stack *ps);

//Pops the last element.
//Pre:Stack is not empty.
void Pop(uint8_t *value, Stack *ps);

//Checks if stack is full.
//Pre:Stack is initialized.
int8_t  StackFull(Stack *ps);

//Checks if stack is empty.
//Pre:Stack is initialized.
int8_t  StackEmpty(Stack *ps);

//Returns the top element
//Pre:Stack is not empty.
void StackTop(uint8_t *value, Stack *ps);

//Returns the StackSize
uint16_t StackSize(Stack *ps);


//Clears the stack.
//Pre:Stack is not empty.
//Pos:Stack is empty.
void ClearStack(Stack *);

//Traverse elements
//Pre:Stack is not empty.
void TraverseStack(Stack *, void (*pfun) (void*));

#endif



