#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//
// Nome: Bruno Cesar de Lima de Paula
// NUSP: 8516351
//

// When in debug mode we print the arrays and matrixes for better understanding of execution
bool flagDebugMode = true;

int numValues;
int numNodes;
int numLeafs;


int operated;


struct treeNode {
   int * values;
   struct treeNode ** nodes;
};

typedef struct treeNode node;

node * rootNode;


void commandInsert(int targetKey) {
	if (flagDebugMode) {
		printf("Inserindo %d!", targetKey);
	}

}


void commandRemove(int targetKey) {
	if (flagDebugMode) {
		printf("Removendo %d!", targetKey);
	}

}


void commandPrint() {
	if (flagDebugMode) {
		printf("Test print...");
	}

}


// Allocate through malloc the needed resources
void createTree() {
	rootNode = (node *)malloc(sizeof(node));
}


// Destroy all malloc allocated resources
void destroyTree() {
	free(rootNode);
}


// Run user commands
void runUserIterations() {
	char operation[60];
	int key = 0;

	while (scanf("%s %d", operation, &key) != EOF) {
		if (strcmp("fim", operation) == 0) {
			break;

		} else if (strcmp("insere", operation) == 0) {
			commandInsert(key);

		} else if (strcmp("remove", operation) == 0) {
			commandRemove(key);

		} else if (strcmp("print", operation) == 0) {
			commandPrint();
		}

	}

}


// The magic starts here!
int main(int numParams, char *params[]){
	char *arquivoEntrada = params[1];
	char *arquivoSaida = params[2];

	createTree();

	runUserIterations();

	destroyTree();

	return 0;
}