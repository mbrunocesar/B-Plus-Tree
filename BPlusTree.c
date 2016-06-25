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

struct treeNode {
	int * numKeys;

	int * values;
	struct treeNode ** nodes;
};

typedef struct treeNode typeNode;

typeNode * root;


// Creates a new node
typeNode * createNode(int baseValue) {
	// used to allow the node to overflow before splitting it
	int temporaryOverflowValue = 2;

	typeNode * newNode = (typeNode *)malloc(sizeof(typeNode));
	newNode->nodes = (typeNode **)malloc(sizeof(typeNode) * (maxOrder + 1 + temporaryOverflowValue));

	newNode->values = (int *)malloc(sizeof(int) * (maxOrder + temporaryOverflowValue));
	newNode->values[0] = baseValue;

	newNode->numKeys = (int *)malloc(sizeof(int));
	newNode->numKeys[0] = 0;

	return newNode;
}

// Destroy a node and all his descendants
void destroyNode(typeNode * node) {
	int position = 0;

	int numValues = node->numKeys[0];

	free(node->values);

	// recursively free memory from alocated nodes
	if (numValues > 0) {
		destroyNode(node->nodes[0]);

		position++;
		while (position <= numValues) {
			destroyNode(node->nodes[position]);
			position++;
		}
	}
	free(node->nodes);
	free(node->numKeys);
	free(node);

}


//
// KEY POSITION METHODS
//
// Find key insert position 
int findKeyInsertPosition(typeNode * node, int key) {
	int lastIndex = node->numKeys[0];

	int position = 0;
	while (key > node->values[position] && position < lastIndex) {
		position = position + 1;
	}
	return position;
}

// Find key position or -1 for failure 
int findKeyPosition(typeNode * node, int key) {
	int keyPosition = -1;
	int lastIndex = node->numKeys[0];

	int position = 0;
	while (position < lastIndex) {
		if (node->values[position] == key) {
			keyPosition = position;
			break;
		}

		position = position + 1;
	}
	return keyPosition;
}


// Shift positions to use a tree with array - Right is used for insertions
void shiftRightPositions(typeNode * node, int startPosition, int endPosition) {
	while (endPosition >= startPosition) {
		node->values[endPosition+1] = node->values[endPosition];
		node->nodes[endPosition+1] = node->nodes[endPosition];
		endPosition--;
	}
}

// Shift positions to use a tree with array - Right is used for deletions
void shiftLeftPositions(typeNode * node, int startPosition, int endPosition) {
	while (endPosition >= startPosition) {
		node->values[startPosition] = node->values[startPosition+1];
		node->nodes[startPosition] = node->nodes[startPosition+1];
		startPosition++;
	}
}



bool isEmptyNode(typeNode * leave) {
	return leave->numKeys[0] == 0;
}


//
// START INSERTION METHODS
//
void insertInEmptyLeave(typeNode * leave, int key) {
	leave->nodes[0] = createNode((1)*100);
	leave->nodes[1] = createNode((2)*100);

	leave->values[0] = key;
}

void insertInLeaveLastPosition(typeNode * leave, int key) {
	int lastIndex = leave->numKeys[0];

	leave->nodes[lastIndex+1] = createNode((lastIndex+2)*100);

	leave->values[lastIndex] = key;
}

void insertInLeavePosition(typeNode * leave, int key, int index) {
	int lastIndex = leave->numKeys[0];

	shiftRightPositions(leave, index, lastIndex+1);

	leave->nodes[index+1] = createNode((lastIndex+2)*100);

	leave->values[index] = key;
}

// Insert a key in a given leave
void insertInLeave(typeNode * leave, int key) {
	int size = leave->numKeys[0];

	if (isEmptyNode(leave)) {
		insertInEmptyLeave(leave, key);

	} else {
		int position = findKeyInsertPosition(leave, key);

		if (position == size) {
			insertInLeaveLastPosition(leave, key);
		} else {
			insertInLeavePosition(leave, key, position);
		}
	}

	leave->numKeys[0] = size + 1;
}


void insertInNode(typeNode * node, int key) {

}


void commandInsert(int key) {
	if (flagDebugMode) {
		printf("Inserindo %d!\n", key);
	}

	insertInLeave(root, key);
}


//
// START REMOTION METHODS
//
void removeUnitaryValue(typeNode * leave) {
	destroyNode(leave->nodes[0]);
	destroyNode(leave->nodes[1]);

	leave->values[0] = 0;
}

void removeFromLeaveLastPosition(typeNode * leave) {
	int lastIndex = leave->numKeys[0];

	destroyNode(leave->nodes[lastIndex]);

	leave->values[lastIndex-1] = 0;
}

void removeFromLeavePosition(typeNode * leave, int index) {
	int lastIndex = leave->numKeys[0];

	leave->values[index] = 0;

	destroyNode(leave->nodes[index]);

	shiftLeftPositions(leave, index, lastIndex);
}

// Insert a key in a given leave
void removeFromLeave(typeNode * leave, int key) {
	int size = leave->numKeys[0];

	if (size == 1) {
		removeUnitaryValue(leave);

	} else {
		int position = findKeyPosition(leave, key);

		if (position == -1) {
			printf("Key is not in the list!\n");

			// avoid over decreasing size
			size = size + 1;
		} else if (position == size - 1) {
			printf("Removing last\n");
			removeFromLeaveLastPosition(leave);
		} else {
			printf("Position %d\n", position);
			removeFromLeavePosition(leave, position);
		}
	}

	leave->numKeys[0] = size - 1;
}

void removeFromNode(typeNode * node, int key) {



}


void commandRemove(int key) {
	if (flagDebugMode) {
		printf("Removendo %d!\n", key);
	}

	removeFromLeave(root, key);
}


//
// START PRINT METHODS
//
void printNode(typeNode * node) {
	int position = 0;
	int numValues = node->numKeys[0];

	printf("Keys: ");
	while (position < numValues) {
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