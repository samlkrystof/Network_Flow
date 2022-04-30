/* ____________________________________________________________________________

    Module hashTable.c
    This module contains functions which work with hashTable struct made with
    arrayLists. It can be used only by graphNode or graphEdge structs, because
    they have similar structure. HashTable saves only pointers to structs.
   ____________________________________________________________________________
*/
#include <malloc.h>
#include <stdio.h>
#include "hashTable.h"
#include "structs.h"

/* ____________________________________________________________________________

    hashTable *createHashtable(int size, int itemSize)

    Creates new hashTable with which has size same as parameter size and
    itemSize. HashTable is made with usage of arrayLists.
   ____________________________________________________________________________
*/
hashTable *createHashTable(int size, int itemSize) {
    int i;
    hashTable *table = malloc(sizeof(hashTable));
    table->size = size;
    table->itemSize = itemSize;
    table->filledItems = 0;
    table->array = malloc(sizeof(arrayList *) * size);
    for (i = 0; i < table->size; i++) {
        table->array[i] = createArrayList(1, table->itemSize);
    }

    return table;
}

/* ____________________________________________________________________________

    int hashTableAddElement(void *element, int id, hashTable *table)

    Checks if element with key id is present in the hashTable, if it is, zero
    is returned and element is not added, in the other case element is added
    and 1 is returned
   ____________________________________________________________________________
*/
int hashTableAddElement(void *element, int id, hashTable *table) {
    int i;
    int index;
    void *hashElement;
    index = ABS(id % table->size);

    for (i = 0; i < table->array[index]->filledItems; i++) {
        hashElement = arrayListGetPointer(table->array[index],0);
        if (((graphNode *)hashElement)->id == id) return 0;
    }

    arrayListAdd(table->array[index], element);
    table->filledItems++;

    return 1;
}

/* ____________________________________________________________________________

    int hashTableAddUncheckedElement(void *element, int index, hashTable *table)

    Adds the element without checking if the index is unique in the hashTable,
    this function is used when new edges are added into graph on index which
    means the source node of the edge
   ____________________________________________________________________________
*/
int hashTableAddUncheckedElement(void *element, int index, hashTable *table) {
    int updatedIndex;
    if (!element || !table) return 0;

    updatedIndex = ABS(index % table->size);
    arrayListAdd(table->array[updatedIndex], element);
    table->filledItems++;

    return 1;
}

/* ____________________________________________________________________________

    void *hashTableGetElement(int id, hashTable *table)

    Returns element with specified id, or NULL if the element is not present.
   ____________________________________________________________________________
*/
void *hashTableGetElement(int id, hashTable *table) {
    int i;
    int index;
    void *element;

    index = ABS(id % table->size);

    for (i = 0; i < table->array[index]->filledItems; i++) {
        element = arrayListGetPointer(table->array[index], i);
        if (id == ((graphNode *)element)->id) {
            return element;
        }
    }

    return NULL;
}

/* ____________________________________________________________________________

    int hashTableContains(int id, hashTable *table)

    Returns whether the table contains element with key id.
   ____________________________________________________________________________
 */
int hashTableContains(int id, hashTable *table) {
    int i;
    int index;

    if (!table) return 0;

    index = ABS(id % table->size);
    for (i = 0; i < table->array[index]->filledItems; i++) {
        if (((graphNode *)arrayListGetPointer(table->array[index],i))->id == id) {
            return 1;
        }
    }

    return 0;
}

/* ____________________________________________________________________________

    void freeHashTable(hashTable **table)

    Deallocates memory used by hashTable, arrayLists which it uses and elements
    saved in arrayLists.
   ____________________________________________________________________________
 */
void freeHashTable(hashTable **table) {
    int i;
    int j;
    graphEdge *pointer;

    if (!table || !*table) return;

    for (i = 0; i < (*table)->size; i++) {
        if ((*table)->array[i]) {
            for (j = 0; j < (*table)->array[i]->filledItems; j++) {
                pointer = arrayListGetPointer((*table)->array[i], j);
                if (pointer) {
                    if (pointer->wkt) free(pointer->wkt);
                    free(pointer);
                }
            }
            freeArrayList(&(*table)->array[i]);
        }
    }
    free((*table)->array);
    free(*table);
    *table = NULL;
}