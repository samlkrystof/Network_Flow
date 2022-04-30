/* ____________________________________________________________________________

    Module graph.c
    This module contains functions which work with graph struct and with
    graphEdge struct. It also contains algorithm to find maximal network flow.
    Dinics algorithm is used.
   ____________________________________________________________________________
*/

#include "graph.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* ____________________________________________________________________________

    long remainingCapacity(graphEdge *edgePointer)

    Returns remaining capacity of the graphEdge. If graphEdge is NULL returns -1.
   ____________________________________________________________________________
*/
long remainingCapacity(graphEdge *edgePointer) {
    if (!edgePointer) return -1;
    return edgePointer->capacity - edgePointer->flow;
}

/* ____________________________________________________________________________

    void augment(graphEdge *edgePointer, long bottleNeck)

    Updates flow in the graphEdge and in the reverseEdge.
   ____________________________________________________________________________
*/
void augment(graphEdge *edgePointer, long bottleNeck) {
    if (!edgePointer || !edgePointer->reverseEdge) return;

    edgePointer->flow += bottleNeck;
    ((graphEdge *)edgePointer->reverseEdge)->flow -= bottleNeck;
}


/* ____________________________________________________________________________

    graphEdge *createReverseEdge(uint from, uint to)

    Tries to create graphEdge reverse with -1 id and zero capacity. If
    it's successful, it returns pointer to new graphEdge, else it returns NULL.
   ____________________________________________________________________________
*/
graphEdge *createReverseEdge(int from, int to) {
    graphEdge *temp = calloc(1, sizeof(graphEdge));
    if (!temp) return NULL;

    temp->source = from;
    temp->target = to;
    temp->capacity = 0;
    temp->id = -1;
    return temp;
}

/* ____________________________________________________________________________

    graph *createNewGraph(arrayList *nodesList, arrayList *edgesTable)

    Creates new graph from arrayLists which contain graphNodes and graphEdges.
    Sets up everything which is needed to run maximal flow algorithm. Returns
    pointer to the graph or NULL if error occurred.
   ____________________________________________________________________________
*/
graph *createNewGraph(hashTable *nodesTable, hashTable *edgesTable) {
    int i;
    int j;
    graphEdge *edgePointer;
    graph *graphPointer;

    if (!nodesTable || !edgesTable || (nodesTable->itemSize != sizeof(graphNode))
        || (edgesTable->itemSize != sizeof(graphEdge))) return NULL;

    graphPointer = calloc(1, sizeof(graph));
    if (!graphPointer) return NULL;

    graphPointer->nodes = nodesTable;
    graphPointer->edges = createHashTable(nodesTable->size, sizeof(graphEdge));
    if (!graphPointer->edges) {
        freeNewGraph(&graphPointer);
        return NULL;
    }

    graphPointer->maxFlow = 0;
    graphPointer->source = -1;
    graphPointer->target = -1;

    for (i = 0; i < edgesTable->size; i++) {
        for (j = 0; j < edgesTable->array[i]->filledItems; j++) {
            edgePointer = arrayListGetPointer(edgesTable->array[i], j);
            if (edgePointer) {
                graphAddEdge(edgePointer, graphPointer);
            }
        }
    }

    /* all edges are sorted, because in hashtable there can be edges with different source
       nodes on the same place */
    for (i = 0; i < graphPointer->edges->size; i++) {
        sortArrayList(graphPointer->edges->array[i]);
    }

    return graphPointer;
}

/* ____________________________________________________________________________

    void graphAddEdge(graphEdge *oldEdge, graph *graphPointer)

    Creates newEdge, same as oldEdge, creates reverse edge to the newEdge and
    adds both in the graph.
   ____________________________________________________________________________
*/
void graphAddEdge(graphEdge *oldEdge, graph *graphPointer) {
    graphEdge *newEdge;
    graphEdge *reverseEdge;
    if (!oldEdge || !graphPointer) return;
    newEdge = calloc(1, sizeof (graphEdge));
    if (!newEdge) return;

    newEdge->id = oldEdge->id;
    newEdge->source = oldEdge->source;
    newEdge->target = oldEdge->target;
    newEdge->capacity = oldEdge->capacity;
    newEdge->isValid = oldEdge->isValid;
    newEdge->wkt = calloc(1, strlen(oldEdge->wkt) + 1);
    strncpy(newEdge->wkt, oldEdge->wkt, strlen(oldEdge->wkt));

    reverseEdge = createReverseEdge(newEdge->target, newEdge->source);
    if (!reverseEdge) return;

    newEdge->reverseEdge = (struct graphEdge *) reverseEdge;
    reverseEdge->reverseEdge = (struct graphEdge *) newEdge;

    hashTableAddUncheckedElement(newEdge, newEdge->source, graphPointer->edges);
    hashTableAddUncheckedElement(reverseEdge, reverseEdge->source, graphPointer->edges);
}

/* ____________________________________________________________________________

    long graphMaxFlow(graph *graphPointer, int source, int target)

    Finds maximal flow between source and target nodes.
   ____________________________________________________________________________
*/
long graphMaxFlow(graph *graphPointer, int source, int target) {
    if (!graphPointer) return -1;

    if (source < 0 || target < 0) return -1;

    graphPointer->maxFlow = 0;
    graphPointer->source = source;
    graphPointer->target = target;

    graphSolve(graphPointer);

    return graphPointer->maxFlow;
}

/* ____________________________________________________________________________

    void graphSolve(graph *graphPointer)

    Runs the Dinics algorithm to find maximal flow from source to target
    (both nodes defined in graph struct).
   ____________________________________________________________________________
*/
void graphSolve(graph *graphPointer) {
    int i;
    int j;
    long f;
    graphNode *node;
    intQueue *queue;

    if (!graphPointer || graphPointer->source == -1 || graphPointer->target == -1) return;
    queue = createIntQueue(INITIAL_SIZE);

    if (!queue) return;

    /* graphBfs function creates level graph, and checks if sink is reachable */
    while (graphBfs(graphPointer, queue)) {

        /* reset next variable of all nodes to initial state */
        for (i = 0; i < graphPointer->nodes->size; i++) {
            for (j = 0; j < graphPointer->nodes->array[i]->filledItems; j++) {
                node = arrayListGetPointer(graphPointer->nodes->array[i], j);
                if (node) node->next = 0;
            }
        }

        /* graphDfs function finds augmenting paths, until blocking flow is reached,
           it means that with current level graph configuration, we can't find another
           augmenting paths, so the outer loop must run again, and create level graph
           again */

        do {
            f = graphDfs(graphPointer->source, graphPointer, INF);
            graphPointer->maxFlow += f;
        } while (f);
    }
    intQueueFreeQueue(&queue);
}

/* ____________________________________________________________________________

    int graphBfs(graph *graphPointer, intQueue *queue)

    At first, changes level of all nodes to -1 (makes the nodes unvisited).
    Then creates "levels" in the graph. Returns boolean value if there is some
    path to augment which leads from source node to target node.

   ____________________________________________________________________________
*/
int graphBfs(graph *graphPointer, intQueue *queue) {
    int i;
    int j;
    int index;
    graphNode *targetNode;
    long capacity;
    int *nodeId = malloc(sizeof(int));
    graphEdge *loadedEdgePointer;

    if (!graphPointer || !nodeId || !queue) return 0;

    /* mark all nodes unvisited */
    for (i = 0; i < graphPointer->nodes->size; i++) {
        for (j = 0; j < graphPointer->nodes->array[i]->filledItems; j++) {
            targetNode = arrayListGetPointer(graphPointer->nodes->array[i], j);
            if (targetNode) targetNode->level = -1;
        }
    }
    /* mark source nodeId visited */
    targetNode = hashTableGetElement(graphPointer->source, graphPointer->nodes);
    targetNode->level = 0;

    intQueueEnque(queue, graphPointer->source);

    while (!intQueueIsEmpty(queue)) {
        /* this if statement should never be true, that will mean error in queue */
        if (intQueueDeque(nodeId, queue) == -1) return 0;

        /* make index fit to hashtable */
        index = ABS(*nodeId % graphPointer->edges->size);

        for (i = 0; i < graphPointer->edges->array[index]->filledItems; i++)    {
            /*go through all the edges with source in nodeId (*nodeId)*/
            loadedEdgePointer = (graphEdge *) arrayListGetPointer(graphPointer->edges->array[index], i);

            /* in the hashtable is possible to have more nodes in one place */
            if (loadedEdgePointer->source < *nodeId) continue;
            if (loadedEdgePointer->source > *nodeId) break;

            capacity = remainingCapacity(loadedEdgePointer);

            /* if the edge has some remaining capacity and if the target nodeId of the edge was not
               visited yet */
            targetNode = hashTableGetElement(loadedEdgePointer->target, graphPointer->nodes);
            if (targetNode->level == -1 && capacity > 0) {

                /* mark as visited */
                targetNode->level = ((graphNode *) hashTableGetElement(*nodeId, graphPointer->nodes))->level + 1;

                intQueueEnque(queue, loadedEdgePointer->target);
            }
        }
    }
    free(nodeId);
    /* return boolean value whether sink nodeId was reached (visited) */
    return ((graphNode *)hashTableGetElement(graphPointer->target, graphPointer->nodes))->level != -1;
}

/* ____________________________________________________________________________

    long graphDfs(unsigned int node, graph *graphPointer, long flow)

    RECURSIVE FUNCTION
    Finds augmenting path from source to target. Augments all the edges in
    the path. Removes dead ends by increasing next variable. Returns the
    augmenting value.
   ____________________________________________________________________________
*/
long graphDfs(int node, graph *graphPointer, long flow) {
    int index;
    int sourceNodeNext;
    uint edgesCount;
    long capacity;
    long bottleNeck;

    graphEdge *loadedEdgePointer;
    graphNode *sourceNode;
    graphNode *targetNode;

    if (!graphPointer) return 0;
    /* sink node was reached */
    if (node == graphPointer->target) return flow;

    /* make node index fit the hashtable */
    index = ABS(node % graphPointer->edges->size);

    edgesCount = graphPointer->edges->array[index]->filledItems;
    sourceNode = hashTableGetElement(node, graphPointer->nodes);


    if (!sourceNode) return 0;

    /* local copy of sourceNode->next, sourceNode->next can't be directly used because
       hashtable can contain two nodes with different IDs in one place */
    sourceNodeNext = sourceNode->next;

    for (; sourceNodeNext < edgesCount; sourceNodeNext++) {
        loadedEdgePointer = (graphEdge *) arrayListGetPointer(graphPointer->edges->array[index], sourceNodeNext);

        if (!loadedEdgePointer) return 0;

        /* in case that in hashtable edges there are more nodes on the same index */
        if (loadedEdgePointer->source < sourceNode->id) continue;
        if (loadedEdgePointer->source > sourceNode->id) break;

        capacity = remainingCapacity(loadedEdgePointer);

        targetNode = hashTableGetElement(loadedEdgePointer->target, graphPointer->nodes);

        if (!targetNode) return 0;

        /* capacity of the edge must be greater than zero and we want only to take edges,
           which can bring us "deeper" in the graph, so their level must be greater than
           current level */
        if (capacity > 0 && sourceNode->level + 1 == targetNode->level) {
            bottleNeck = graphDfs(loadedEdgePointer->target, graphPointer, MIN(flow, capacity));

            if (bottleNeck > 0) {
                augment(loadedEdgePointer, bottleNeck);
                return bottleNeck;
            }
        }

        /* sourceNode->next++ removes the dead end, for example, if value at
       sourceNode->next was 0, and sink node was not reached, sourceNode->next
       will be incremented to 1 and the edge on index 0 will not be used in
       next iterations of dfs */
        sourceNode->next++;
    }

    /* sink node was not reached */
    return 0;
}

/* ____________________________________________________________________________

    void freeNewGraph(graph **graphPointer)

    Deallocates memory used by graph.
   ____________________________________________________________________________
*/
void freeNewGraph(graph **graphPointer) {

    if (!graphPointer || !*graphPointer) return;

    freeHashTable(&(*graphPointer)->edges);

    free(*graphPointer);
    *graphPointer = NULL;
}
