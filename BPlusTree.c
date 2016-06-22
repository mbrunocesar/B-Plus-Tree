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

int maxOrder = 4;

int totalNumValues;
int totalNumNodes;
int totalNumLeafs;

int operated;

struct treeNode {
	int * numKeys;

	int * values;
	struct treeNode ** nodes;
};

typedef struct treeNode typeNode;

typeNode * root;


// Creates a new node
typeNode * createNode(int baseValue) {
	typeNode * newNode = (typeNode *)malloc(sizeof(typeNode));
	newNode->nodes = (typeNode **)malloc(sizeof(typeNode) * (maxOrder+1));

	newNode->values = (int *)malloc(sizeof(int) * (maxOrder));
	newNode->values[0] = baseValue;

	newNode->numKeys = (int *)malloc(sizeof(int));
	newNode->numKeys[0] = 0;

	return newNode;
}

// Creates a node all his descendants
void destroyNode(typeNode * node) {
	int position = 0;

	int numValues = node->numKeys[0];

	free(node->values);
	// recursively free memory from alocated nodes
	if (numValues > 0) {
		destroyNode(node->nodes[0]);
		while (position < numValues) {
			destroyNode(node->nodes[position+1]);
			position++;
		}
	}
	free(node->nodes);
	free(node->numKeys);
	free(node);

}


// Insert a key in a given leave
void insertInLeave(typeNode * leave, int key) {
	totalNumValues++;

	int currentIndex = leave->numKeys[0];

	if (flagDebugMode) {
		printf("Current Index %d\n", currentIndex);
	}

	leave->values[currentIndex] = key;
	
	if (currentIndex == 0) {
		leave->nodes[currentIndex] = createNode((currentIndex+1)*100);
	}
	
	leave->nodes[currentIndex+1] = createNode((currentIndex+2)*100);

	leave->numKeys[0] = currentIndex + 1;
}


void commandInsert(int key) {
	if (flagDebugMode) {
		printf("Inserindo %d!\n", key);
	}

	insertInLeave(root, key);

}


void commandRemove(int targetKey) {
	if (flagDebugMode) {
		printf("Removendo %d!\n", targetKey);
	}

}


void printNode(typeNode * node) {
	int position = 0;
	int numValues = node->numKeys[0];

	printf("Keys: ");
	while (position <= numValues) {
		printf("%d ", node->values[position]);
		position++;
	}
	printf("\n");

	if (numValues > 0) {
		position = 0;
		printf("Sub Keys: ");
		while (position <= numValues) {
			printf("%d ", node->nodes[position]->values[0]);
			position++;
		}
		printf("\n");
	}
}

void commandPrint() {
	printNode(root);
}


// Allocate through malloc the needed resources
void createTree() {
	root = createNode(19);

	totalNumValues = 0;
	totalNumNodes = 1;
	totalNumLeafs = 1;
}

// Destroy all malloc allocated resources
void destroyTree() {
	destroyNode(root);
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
int main(){

	createTree();

	runUserIterations();

	destroyTree();

	return 0;
}