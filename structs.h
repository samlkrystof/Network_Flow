#ifndef SEMESTRALKA_STRUCTS_H
#define SEMESTRALKA_STRUCTS_H

#include "hashTable.h"

/* ____________________________________________________________________________

    Structures and Datatypes
   ____________________________________________________________________________
*/

typedef struct {
    char *wkt;
    int id;
    int next;
    int level;
} graphNode;

typedef struct {
    char *wkt;
    int id;
    char isValid;
    int source;
    int target;
    long flow;
    long capacity;
    struct graphEdge *reverseEdge;
} graphEdge;

typedef struct {
    hashTable *nodes;
    hashTable *edges;
    long maxFlow;
    int source;
    int target;
} graph;
#endif
