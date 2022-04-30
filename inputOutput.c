/* ____________________________________________________________________________

    Module inputOutput.c
    This module contains functions which deal with input and output. Also it
    contains functions to create edge, node, and functions to deallocate edge,
    node, and arraylists containing them.
   ____________________________________________________________________________
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "arrayList.h"
#include "structs.h"
#include "inputOutput.h"

/* ____________________________________________________________________________

    graphNode *createNode(char *input)

    Tries to create node from input string. If it is successful it returns
    pointer on new node, else NULL.
   ____________________________________________________________________________
 */
graphNode *createNode(char *input) {
    graphNode *newNode;
    char *inputPart;
    char *invalidPart;

    if (!input || strlen(input) < 5) return NULL;

    newNode = malloc(sizeof(graphNode));
    if (!newNode) return NULL;

    inputPart = strtok(input, ",");
    newNode->id = strtol(inputPart, &invalidPart, 10);

    inputPart = strtok(NULL, ",");
    newNode->wkt = malloc(strlen(inputPart));

    if (!newNode->wkt) {
        free(newNode);
        return NULL;
    }
    strncpy(newNode->wkt, inputPart, strlen(inputPart));
    return newNode;
}

/* ____________________________________________________________________________

    graphEdge *createEdge(char *input, int workWithInvalid)

    Tries to create edge from input string. Checks if edge is valid, if it is
    returns pointer to new edge, else return NULL.
   ____________________________________________________________________________
 */
graphEdge *createEdge(char *input, int workWithInvalid) {
    graphEdge *newEdge;
    int i;
    uint wktLength;
    uint inputLength;
    uint parsedLength;
    char *partString;
    char *part2;

    if (!input || strlen(input) < 14) return NULL;
    inputLength = strlen(input);
    parsedLength = 5;

    newEdge = calloc(1, sizeof(graphEdge));
    if (!newEdge) return NULL;

    partString = strtok(input, ",");
    for (i = 0; i < 4; i++) {
        switch (i) {
            case 0:
                newEdge->id = strtol(partString, &part2, 10);
                break;

            case 1:
                newEdge->source = strtol(partString, &part2, 10);
                break;

            case 2:
                newEdge->target = strtol(partString, &part2, 10);
                break;

            case 3:
                if (strchr(partString, '"')) {
                    newEdge->capacity = strtol(&partString[1], &part2, 10);
                } else {
                    newEdge->capacity = strtol(partString, &part2, 10);
                }
                break;

            default:
                perror("Unexpected error has occurred\n");
        }

        parsedLength += strlen(partString);
        partString = strtok(NULL, ",");
    }

    if (!strncmp(partString, "True", 4)) {
        newEdge->isValid = 'Y';
    } else {

        if (!workWithInvalid) {
            freeGraphEdge(&newEdge);
            return NULL;
        }
        newEdge->isValid = 'N';
    }

    parsedLength += strlen(partString);
    partString = strtok(NULL, "&");
    wktLength = inputLength - parsedLength + 1;
    newEdge->wkt = malloc(wktLength);

    strncpy(newEdge->wkt, partString, wktLength);
    newEdge->flow = 0;

    return newEdge;
}

/* ____________________________________________________________________________

    hashTable *loadNodes(char *inputName)

    Tries to load data from file described by inputName parameter and create
    hashTable containing newly created nodes, it filters duplicate nodes.
    Returns pointer to hashTable or NULL if some problem occurred.
   ____________________________________________________________________________
 */
hashTable *loadNodes(char *inputName) {
    int counter = 0;
    hashTable *table;
    char *string;
    graphNode *newNode;
    FILE *inputFile;

    if (!inputName) return NULL;

    inputFile = fopen(inputName, "r");
    if (!inputFile) return NULL;

    string = malloc(MAXLENGTH);

    if (!string) {
        freeHashTable(&table);
        fclose(inputFile);
        return NULL;
    }

    fgets(string, MAXLENGTH, inputFile);
    if (strncmp(NODES_ID, string, strlen(NODES_ID)) != 0) {
        free(string);
        freeHashTable(&table);
        fclose(inputFile);
        return NULL;
    }

    do {
        fgets(string, MAXLENGTH, inputFile);
        counter++;
    } while (!feof(inputFile));
    rewind(inputFile);
    fgets(string, MAXLENGTH, inputFile);

    table = createHashTable(counter + 2, sizeof(graphNode));
    if (!table) {
        fclose(inputFile);
        return NULL;
    }

    do {
        fgets(string, MAXLENGTH, inputFile);
        newNode = createNode(string);
        if (newNode) {
            /* check if node with this id was already loaded */
            if (hashTableContains(newNode->id, table)) {
                freeGraphNode(&newNode);
            } else {
                hashTableAddElement(newNode, newNode->id, table);
            }
        }
    } while (!feof(inputFile));

    fclose(inputFile);
    free(string);
    return table;
}

/* ____________________________________________________________________________

    hashTable *loadEdges(char *inputName, int workWithInvalid, int tableSize)

    Tries to load data from file described by inputName parameter and create
    hashTable containing newly created edges, it filters duplicates.
    WorkWithInvalid says if invalid edges should be used, tableSize determines
    size of the hashTable. Returns pointer to hashTable or NULL if some problem
    occurred.
   ____________________________________________________________________________
 */

hashTable *loadEdges(char *inputName, int workWithInvalid, int tableSize) {
    hashTable *table;
    char *string;
    graphEdge *edgePointer;
    FILE *inputFile;
    if (!inputName) return NULL;

    inputFile = fopen(inputName, "r");
    if (!inputFile) return NULL;

    table = createHashTable(tableSize, sizeof(graphEdge));
    if (!table) {
        fclose(inputFile);
        return NULL;
    }
    string = malloc(MAXLENGTH);
    if (!string) {
        freeHashTable(&table);
        fclose(inputFile);
        return NULL;
    }

    fgets(string, MAXLENGTH, inputFile);
    if (strncmp(EDGES_ID, string, strlen(EDGES_ID)) != 0) {
        free(string);
        freeHashTable(&table);
        fclose(inputFile);
        return NULL;
    }

    do {
        fgets(string, MAXLENGTH, inputFile);
        edgePointer = createEdge(string, workWithInvalid);
        if (edgePointer) {
            /* check if edge with this id was already loaded */
            if (hashTableContains(edgePointer->id, table)) {
                freeGraphEdge(&edgePointer);
            } else {
                hashTableAddElement(edgePointer, edgePointer->id, table);
            }
        }
    } while (!feof(inputFile));

    fclose(inputFile);
    free(string);

    return table;
}

/* ____________________________________________________________________________

    void freeGraphNode(graphNode **nodePointer)

    Deallocates memory used by node.
   ____________________________________________________________________________
*/
void freeGraphNode(graphNode **nodePointer) {
    if (!nodePointer || !*nodePointer) return;

    if ((*nodePointer)->wkt) free((*nodePointer)->wkt);
    free(*nodePointer);
    *nodePointer = NULL;
}

/* ____________________________________________________________________________

    void freeGraphEdge(graphEdge **edgePointer)

    Deallocates memory used by edge.
   ____________________________________________________________________________
*/
void freeGraphEdge(graphEdge **edgePointer) {
    if (!edgePointer || !*edgePointer) return;
    if ((*edgePointer)->wkt) free((*edgePointer)->wkt);
    free(*edgePointer);
    *edgePointer = NULL;
}

/* ____________________________________________________________________________

    int writeToOutputFile(char *fileName, graph *graphPointer)

    Tries to write output to file described by fileName, writes all edges in
    the min-cut. Then returns if it was successful as boolean value.
   ____________________________________________________________________________
*/
int writeToOutputFile(char *fileName, graph *graphPointer) {
    int i;
    int j;
    graphEdge *edgePointer;
    graphNode *sourceNode;
    graphNode *targetNode;
    char *validity;
    arrayList *list;
    FILE *output;

    if (!fileName || !graphPointer) return FAILURE;

    output = fopen(fileName, "w");
    if (!output) return FAILURE;

    list = createArrayList(INITIAL_SIZE, sizeof(graphEdge));
    if (!list) {
        fclose(output);
        return FAILURE;
    }

    fprintf(output,"%s", EDGES_ID);
    for (i = 0; i < graphPointer->edges->size; i++) {
        for (j = 0; j < graphPointer->edges->array[i]->filledItems; j++) {
            edgePointer = arrayListGetPointer(graphPointer->edges->array[i], j);
            if (edgePointer) {
                /* get rid of reverse edges, and edges which don't have fulfilled capacity */
                if (edgePointer->capacity == 0 || edgePointer->capacity != edgePointer->flow) continue;
                sourceNode = hashTableGetElement(edgePointer->source, graphPointer->nodes);
                targetNode = hashTableGetElement(edgePointer->target, graphPointer->nodes);
                /* nodes which are "to the left" from the min-cut have level != -1,
                  nodes which are "to the right" from the min-cut have level == -1,
                  so we want to write just the edges, which have source in the
                  left side, and target in the right side or edges which have source in the right side and
                  target in the left side*/

                if ((sourceNode->level != -1 && targetNode->level == -1) || (sourceNode->level == -1
                                                                             && targetNode->level != -1)) {
                    arrayListAdd(list, edgePointer);
                }
            }
        }
    }

    qsort(list->data, list->filledItems, sizeof(graphEdge *), compareEdgeById);
    for (i = 0; i < list->filledItems; i++) {
        edgePointer = arrayListGetPointer(list, i);
        validity = edgePointer->isValid == 'Y' ? VALID : INVALID;
        fprintf(output, "%d,%d,%d,%ld,%s,%s", edgePointer->id, edgePointer->source, edgePointer->target,
                edgePointer->capacity, validity, edgePointer->wkt);
    }
    freeArrayList(&list);

    fclose(output);
    return SUCCESS;
}

/* ____________________________________________________________________________

    int compareEdgeById(const void *a, const void *b)

    Auxiliary function which defines how two edges should be compared by id.
    Necessary for using qsort.
   ____________________________________________________________________________
*/
int compareEdgeById(const void *a, const void *b) {
    return (*(graphEdge **) a)->id - (*(graphEdge **) b)->id;
}

/* ____________________________________________________________________________

    int compareEdgeBySource(const void *a, const void *b)

    Auxiliary function which defines how two edges should be compared by source.
    Necessary for using qsort.
   ____________________________________________________________________________
*/
int compareEdgeBySource(const void *a, const void *b) {
    return (*(graphEdge **) a)->source - (*(graphEdge **) b)->source;
}