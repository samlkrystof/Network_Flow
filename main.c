/*
    Module main.c
    This module is entry point to this application. It's responsible for
    user interaction, input validation, and running the programme.
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "arrayList.h"
#include "hashTable.h"
#include "graph.h"

#define MAXLENGTH 4096
#define VALIDATE_INDEX(x, y) (((x) != (-1)) && (((x) + 1) < (y)))
#define EXIT_SUCCESS 0
#define NODES 0
#define INVALID_EDGES 1
#define EDGES 2
#define SOURCE 3
#define TARGET 4

/* ____________________________________________________________________________

    GLOBAL MODULE VARIABLES
   ____________________________________________________________________________
 */

hashTable *loadedNodes = NULL;
hashTable *loadedEdges = NULL;
graph *graphPointer;

/* ____________________________________________________________________________

    void cleanEverything()

    Cleans all global module variables, if they are not NULL.
   ____________________________________________________________________________
 */
void cleanEverything() {
    if (loadedEdges) freeHashTable(&loadedEdges);
    if (loadedNodes) freeHashTable(&loadedNodes);
    if (graphPointer) freeNewGraph(&graphPointer);
}

/* ____________________________________________________________________________

    int findInInput(const int argc, char *argv[], const char *text)

    Looks for text in argv array, returns index where it is in argv, if it's not
    found returns -1.
   ____________________________________________________________________________
 */
int findInInput(const int argc, char *argv[], const char *text) {
    int i;

    for (i = 1; i < argc; i++) {
        if(strcmp(argv[i], text) == 0) {
            return i;
        }
    }
    return -1;
}
/* ____________________________________________________________________________

    MAIN PROGRAM
   ____________________________________________________________________________
 */

int main(int argc, char *argv[]) {
    int index;
    int j;
    int source;
    int target;
    int workWithInvalid = 0;
    /* all possible switches */
    char *switches[6] = {"-v", "-a", "-e", "-s", "-t", "-out"};

    for (j = 0; j < 5; j++) {
        index = findInInput(argc, argv, switches[j]);

        switch (j) {
            case NODES:
                if (VALIDATE_INDEX(index, argc)) {
                    loadedNodes = loadNodes(argv[index + 1]);
                }
                if (!loadedNodes) {
                    cleanEverything();
                    printf("Invalid vertex file.\n");
                    return 1;
                }
                break;

            case INVALID_EDGES:
                if (index != -1) {
                    workWithInvalid = 1;
                }
                break;

            case EDGES:
                if (VALIDATE_INDEX(index, argc)) {
                    loadedEdges = loadEdges(argv[index + 1], workWithInvalid, loadedNodes->size);
                }
                if (!loadedEdges) {
                    cleanEverything();
                    printf("Invalid edge file.\n");
                    return 2;
                }
                break;

            case SOURCE:
                if (VALIDATE_INDEX(index, argc)) {
                    source = strtol(argv[index + 1], NULL, 10);
                }
                if (!hashTableContains(source, loadedNodes)) {
                    cleanEverything();
                    printf("Invalid source vertex.\n");
                    return 3;
                }
                break;

            case TARGET:
                if (VALIDATE_INDEX(index, argc)) {
                    target = strtol(argv[index + 1], NULL, 10);
                }
                if ((target == source) || !hashTableContains(target, loadedNodes)) {

                    cleanEverything();
                    printf("Invalid sink vertex.\n");
                    return 4;
                }
                break;

            default:
                perror("Unexpected error while finding in input.\n");
        }
    }

    graphPointer = createNewGraph(loadedNodes, loadedEdges);

    if (!graphPointer) {
        cleanEverything();
        printf("Unable to create graph.\n");
        return 7;
    }

    printf("Max network flow is |x| = %ld.\n", graphMaxFlow(graphPointer, source, target));
    if (!graphPointer->maxFlow) {
        cleanEverything();
        return 6;
    }

    index = findInInput(argc, argv, switches[5]);

    if (VALIDATE_INDEX(index, argc)) {
        if (!writeToOutputFile(argv[index + 1], graphPointer)) {

            cleanEverything();
            printf("Invalid output file.\n");
            return 5;
        }
    }


    cleanEverything();
    return EXIT_SUCCESS;
}
