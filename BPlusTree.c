#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//
// Nome: Bruno Cesar de Lima de Paula
// NUSP: 8516351
//


// STRUCTURE DEFINITION
struct treeNode {
	int * numKeys;

	int * values;
	struct treeNode ** nodes;
};

typedef struct treeNode typeNode;


// When in debug mode we print the arrays and matrixes for better understanding of execution
bool flagDebugMode = true;

int maxOrder = 4;
int treeHeight = 1;


typeNode * root;


int numKeys(typeNode * node) {
	return node->numKeys[0];
}

void setNumKeys(typeNode * node, int value) {
	node->numKeys[0] = value;
}


// Creates a new node
typeNode * createNode(int baseValue) {
	// used to allow the node to overflow before splitting it
	int temporaryOverflowValue = 2;

	typeNode * newNode = (typeNode *)malloc(sizeof(typeNode));
	newNode->nodes = (typeNode **)malloc(sizeof(typeNode) * (maxOrder + 1 + temporaryOverflowValue));

	newNode->values = (int *)malloc(sizeof(int) * (maxOrder + temporaryOverflowValue));
	newNode->values[0] = baseValue;

	newNode->numKeys = (int *)malloc(sizeof(int));
	setNumKeys(newNode, 0);

	return newNode;
}

// Destroy a node and all his descendants
void destroyNode(typeNode * node) {
	int position = 0;

	int size = numKeys(node);

	free(node->values);

	// recursively free memory from alocated nodes
	if (size > 0) {
		destroyNode(node->nodes[0]);

		position++;
		while (position <= size) {
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
	int lastIndex = numKeys(node);

	int position = 0;
	while (key > node->values[position] && position < lastIndex) {
		position = position + 1;
	}
	return position;
}

// Find key position or -1 for failure 
int findKeyPosition(typeNode * node, int key) {
	int keyPosition = -1;
	int lastIndex = numKeys(node);

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


//
// START GENERAL METHODS
//
bool isRoot(typeNode * parent) {
	return parent == NULL;
}

bool isLeave(int height) {
	return height == treeHeight;
}

bool isEmptyNode(typeNode * node) {
	return numKeys(node) == 0;
}

bool haveOverflow(typeNode * node) {
	return numKeys(node) > maxOrder; 
}

bool keyIsOnTree(typeNode * node, int key) {
	bool isOnTree = false;

	if (numKeys(node) != 0) {
		if (findKeyPosition(node, key) != -1) {
			isOnTree = true;

		} else {
			int position = findKeyInsertPosition(node, key);
			isOnTree = keyIsOnTree(node->nodes[position], key);
		}
	}

	return isOnTree;
}

//
// START INSERTION METHODS
//
void insertInEmptyLeave(typeNode * leave, int key) {
	leave->nodes[0] = createNode(-1);
	leave->nodes[1] = createNode(-1);

	leave->values[0] = key;
}

void insertInLeaveLastPosition(typeNode * leave, int key) {
	int lastIndex = numKeys(leave);

	leave->nodes[lastIndex+1] = createNode(-1);

	leave->values[lastIndex] = key;
}

void insertInLeavePosition(typeNode * leave, int key, int index) {
	int lastIndex = numKeys(leave);

	shiftRightPositions(leave, index, lastIndex+1);

	leave->nodes[index+1] = createNode(-1);

	leave->values[index] = key;
}


void insertInLeave(typeNode * leave, int key, typeNode * parent) {
	int size = numKeys(leave);

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

	setNumKeys(leave, (size + 1));
}

// Special case when overflowing the root element
void keepOnlyMiddleElementInNode(typeNode * node) {
	typeNode * newNode = createNode(-1);

	int middleIndex = (int) (numKeys(node) / 2);

	insertInLeave(newNode, node->values[middleIndex], NULL);

	int position = 0;
	typeNode * subNode = newNode->nodes[0];
	while (position < middleIndex) {
		insertInNode(subNode, node->values[position]);
		position++;
	}

	position = middleIndex + 1;
	subNode = newNode->nodes[1];
	while (position <= maxOrder) {
		insertInNode(subNode, node->values[position]);
		position++;
	}

	root = newNode;
	treeHeight++;
}

void insertInNode(typeNode * node, int key, typeNode * parent, int height) {
	if (isRoot(parent) && isLeave(height)) {
		insertInLeave(node, key, parent);

		if (haveOverflow(node)) {
			keepOnlyMiddleElementInNode(node);
		}

	} else {
		printf("Inserting outside root\n");
		insertInLeave(node, key, parent);
	}

}


void commandInsert(int key) {
	if (flagDebugMode) {
		printf("Inserindo %d!\n", key);
	}

	if (!keyIsOnTree(root, key)) {
		insertInNode(root, key, NULL, 1);

	} else {
		printf("Error - key %d already on tree!\n", key);
	}
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
	int lastIndex = numKeys(leave);

	destroyNode(leave->nodes[lastIndex]);

	leave->values[lastIndex-1] = 0;
}

void removeFromLeavePosition(typeNode * leave, int index) {
	int lastIndex = numKeys(leave);

	leave->values[index] = 0;

	destroyNode(leave->nodes[index]);

	shiftLeftPositions(leave, index, lastIndex);
}


void removeFromLeave(typeNode * leave, int key) {
	int size = numKeys(leave);

	if (size == 1) {
		removeUnitaryValue(leave);

	} else {
		int position = findKeyPosition(leave, key);

		if (position == -1) {
			printf("Key is not in the list!\n");
			// avoid over decreasing size
			size = size + 1;

		} else if (position == size - 1) {
			if (flagDebugMode) {
				printf("Removing last\n");
			}
			removeFromLeaveLastPosition(leave);

		} else {
			if (flagDebugMode) {
				printf("Removing from Position %d\n", position);
			}
			removeFromLeavePosition(leave, position);
		}
	}

	setNumKeys(leave, (size - 1));
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
	int numValues = numKeys(node);

	printf("Keys: ");
	while (position < numValues) {
		printf("%d ", node->values[position]);
		position++;
	}
	if (position > 0) {
		printf("\n");
	}

	if (numValues > 0) {
		position = 0;

		if (numKeys(node->nodes[0]) > 0) {
			printf("Sub Keys:\n");
			while (position <= numValues) {
				if (node->nodes[position]->numKeys[0] > 0) {
					printNode(node->nodes[position]);
				}
				position++;
			}

			if (position > 0) {
				printf("\n");
			}
		}
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