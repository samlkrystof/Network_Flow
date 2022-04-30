#ifndef SEMESTRALKA_INPUTOUTPUT_H
#define SEMESTRALKA_INPUTOUTPUT_H

#define INITIAL_SIZE 10
#define MAXLENGTH 4096
#define NODES_ID "id,WKT\n"
#define EDGES_ID "id,source,target,capacity,isvalid,WKT\n"
#define VALID "True"
#define INVALID "False"

#include "structs.h"

/* ____________________________________________________________________________

    Function Prototypes
   ____________________________________________________________________________
*/

graphNode *createNode(char *input);
void freeGraphNode(graphNode **nodePointer);
void freeGraphEdge(graphEdge **edgePointer);
graphEdge *createEdge(char *input, int workWithInvalid);
hashTable *loadNodes(char *inputName);
hashTable *loadEdges(char *inputName, int workWithInvalid, int tableSize);
int writeToOutputFile(char *fileName, graph *graphPointer);
int compareEdgeById(const void *a, const void *b);
int compareEdgeBySource(const void *a, const void *b);




#endif
