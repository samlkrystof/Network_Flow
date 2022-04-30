#ifndef GRAPH_C_GRAPH_H
#define GRAPH_C_GRAPH_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "arrayList.h"
#include "inputOutput.h"
#include "intQueue.h"

#define INF (LONG_MAX / 2)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/

graphEdge *createReverseEdge(int from, int to);
long remainingCapacity(graphEdge *edgePointer);
void augment(graphEdge *edgePointer, long bottleNeck);
graph *createNewGraph(hashTable *nodesTable, hashTable *edgesTable);
void graphAddEdge(graphEdge *oldEdge, graph *graphPointer);
long graphMaxFlow(graph *graphPointer, int source, int target);
void graphSolve(graph *graphPointer);
int graphBfs(graph *graphPointer, intQueue *queue);
long graphDfs(int node, graph *graphPointer, long flow);
void freeNewGraph(graph **graphPointer);

#endif