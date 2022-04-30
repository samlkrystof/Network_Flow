/* ____________________________________________________________________________

    Module intQueue.c
    This module contains functions that work with intQueue struct
    implemented by array.

   ____________________________________________________________________________
 */
#include <malloc.h>
#include "intQueue.h"
#define TRUE 1
#define FALSE 0
#define FAIL (-1)

/* ____________________________________________________________________________

    intQueue *createIntQueue(unsigned int capacity)

    Returns pointer to newly created intQueue with initial capacity defined by
    function parameter. May return NULL if it's not possible to alloc data for
    creation.
   ____________________________________________________________________________
 */
intQueue *createIntQueue(unsigned int capacity) {
    intQueue *temp;

    temp = malloc(sizeof(intQueue));
    if (!temp) return NULL;

    temp->capacity = capacity;
    temp->first = 0;
    temp->count = 0;

    temp->array = malloc(capacity * sizeof(int));
    if (!temp->array) {
        free(temp);
        return NULL;
    }

    return temp;
}

/* ____________________________________________________________________________

    void intQueueEnque(intQueue *queue, unsigned int element)

    Adds parameter element to the end of queue, expands queue if it's needed.
   ____________________________________________________________________________
 */
void intQueueEnque(intQueue *queue, int element) {
    if (!queue) return;

    if (intQueueIsFull(queue)) intQueueExpand(queue);

    queue->array[(queue->first + queue->count) % queue->capacity] = element;
    queue->count++;
}

/* ____________________________________________________________________________

    int intQueueDeque(int *intPointer, intQueue *queue)

    Passes first element of queue to the intPointer. Returns 1 if the operation
    was successful, else returns -1.
   ____________________________________________________________________________
 */
int intQueueDeque(int *intPointer, intQueue *queue) {
    if (!queue || intQueueIsEmpty(queue)) return FAIL;

    *intPointer = queue->array[queue->first];
    queue->first = (queue->first + 1) % queue->capacity;
    queue->count--;
    return TRUE;
}

/* ____________________________________________________________________________

    void intQueueExpand(intQueue *queue)
    Doubles the capacity of queue. It can be slow, if queue has many elements
    in it.
   ____________________________________________________________________________
 */
void intQueueExpand(intQueue *queue) {
    int i;
    int *temp;
    unsigned int newCapacity;

    if (!queue || !queue->array) return;

    newCapacity = queue->capacity * 2;
    temp = malloc(newCapacity * sizeof(int));
    if (temp) {

        for (i = 0; i < queue->capacity; i++) {
            temp[i] = queue->array[(queue->first + i) % queue->capacity];
        }
        free(queue->array);
        queue->array = temp;
        queue->capacity = newCapacity;
        queue->first = 0;
    }
    temp = NULL;
}

/* ____________________________________________________________________________

    intQueueIsFull(intQueue *queue)

    Checks if capacity of queue is fully used. Returns boolean value or -1 if
    queue is NULL.
   ____________________________________________________________________________
 */
int intQueueIsFull(intQueue *queue) {
    if (!queue) return FAIL;

    return queue->count == queue->capacity ? TRUE : FALSE;
}

/* ____________________________________________________________________________

    int intQueueIsEmpty(intQueue *queue)

    Check if queue is empty. Returns boolean value or -1 if queue is NULL.

   ____________________________________________________________________________
 */
int intQueueIsEmpty(intQueue *queue) {
    if (!queue) return FAIL;

    return queue->count == 0 ? TRUE : FALSE;
}

/* ____________________________________________________________________________

    void intQueueFreeQueue(intQueue **queue)

    Deallocates memory used by queue.
   ____________________________________________________________________________
 */
void intQueueFreeQueue(intQueue **queue) {
    if(!queue || !*queue) return;

    if ((*queue)->array) free((*queue)->array);

    free(*queue);
    *queue = NULL;
}
