#ifndef SEMESTRALKA_INTQUEUE_H
#define SEMESTRALKA_INTQUEUE_H

/* ____________________________________________________________________________

    Structures and Datatypes
   ____________________________________________________________________________
*/

typedef struct {
    unsigned int capacity;
    unsigned int first;
    int count;
    int *array;
}intQueue;

/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/

intQueue *createIntQueue(unsigned int capacity);
void intQueueEnque(intQueue *queue, int element);
int intQueueDeque(int *intPointer, intQueue *queue);
void intQueueExpand(intQueue * queue);
int intQueueIsFull(intQueue *queue);
int intQueueIsEmpty(intQueue *queue);
void intQueueFreeQueue(intQueue **queue);


#endif
