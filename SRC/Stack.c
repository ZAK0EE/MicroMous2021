#include "Stack.h"

//Initializes the stack.
void CreateStack(Stack *ps)
{
    ps->top = 0;
}

//Push new element to the end of the stack, assuming the stack is not full.
int8_t Push(uint8_t value, Stack *ps)
{
    ps->entry[ps->top++] = value;
    return 1;
}

//Pops the last element
void Pop(uint8_t *value, Stack *ps)
{
    *value = ps->entry[--ps->top];
}

//Checks if stack is full.
int8_t  StackFull(Stack *ps)
{
    return (ps->top == MAX_ELEMENTS);
}

//Checks if stack is empty.
int8_t  StackEmpty(Stack *ps)
{
    return (ps->top == 0);
}

//Returns the top element
void StackTop(uint8_t *value, Stack *ps)
{
    *value = ps->entry[ps->top - 1];
}

//Returns the StackSize
uint16_t StackSize(Stack *ps)
{
    return ps->top;
}

//Clears the stack.
void ClearStack(Stack *ps)
{
    ps->top = 0;
}

//Traverse elements
void TraverseStack(Stack *ps, void (*pfun) (void*))
{
    for (int i = ps->top - 1; i >= 0; i--)
        (*pfun)(&ps->entry[i]);
}

