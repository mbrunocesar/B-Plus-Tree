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

// Root of the tree
typeNode * root;

// Flag activated only for tests
bool flagDebug = false;

// Here it's defined the tree order, works better for even's higher than 2 numbers
int maxOrder = 4;

// Tree Height
int treeHeight = 1;


// Node num keys management methods
int numKeys(typeNode * node) {
	return node->numKeys[0];
}

void setNumKeys(typeNode * node, int value) {
	node->numKeys[0] = value;
}


//
// START GENERAL METHODS
//

// if hasn't any parent it's the tree root
bool isRoot(typeNode * parent) {
	return parent == NULL;
}

// if the node is in the same height than the tree height, then it's a leave
bool isLeave(int height) {
	return height == treeHeight;
}

bool isEmptyNode(typeNode * node) {
	return numKeys(node) == 0;
}

bool haveOverflow(typeNode * node) {
	return numKeys(node) > maxOrder; 
}


// Creates a new node allocating all the needed resources
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

// Destroy a node and all his descendants recursively
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

// Get the value of the last positioned value in the node
int findLastKeyValue(typeNode * node) {
	int lastIndex = numKeys(node);

	return node->values[lastIndex-1];
}



// Shift positions to use a tree with array - Right is used for insertions
void shiftRightPositions(typeNode * node, int startPosition, int endPosition) {
	while (endPosition >= startPosition) {
		node->values[endPosition+1] = node->values[endPosition];
		node->nodes[endPosition+1] = node->nodes[endPosition];
		endPosition--;
	}
}

// Shift positions to use a tree with array - Left is used for deletions
void shiftLeftPositions(typeNode * node, int startPosition, int endPosition) {
	while (endPosition >= startPosition) {
		node->values[startPosition] = node->values[startPosition+1];
		node->nodes[startPosition] = node->nodes[startPosition+1];
		startPosition++;
	}
}


// Check if the key is on tree recursively
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

// The insert in leaves methods sets the value and creates emptys nodes to allow futures insertions
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

// This method insert in a position shifting values to right to allow a positional insert
void insertInPosition(typeNode * leave, int key, int index) {
	int lastIndex = numKeys(leave);

	shiftRightPositions(leave, index, lastIndex+1);

	leave->nodes[index+1] = createNode(-1);

	leave->values[index] = key;
}


// Checks how it's the best way to insert in a given leave and do it
void insertInLeave(typeNode * leave, int key) {
	int size = numKeys(leave);

	if (isEmptyNode(leave)) {
		insertInEmptyLeave(leave, key);

	} else {
		int position = findKeyInsertPosition(leave, key);

		if (position == size) {
			insertInLeaveLastPosition(leave, key);
		} else {
			insertInPosition(leave, key, position);
		}
	}

	setNumKeys(leave, (size + 1));
}

// This method insert some value in a non-leave (comes from promotion)
// In the process it's gets the values bigger than the promoted to a new node
void insertInNonLeave(typeNode * node, int key, int height) {
	int size = numKeys(node);

	int position = findKeyInsertPosition(node, key);

	insertInPosition(node, key, position);

	typeNode * oldNode = node->nodes[position];
	typeNode * newNode = node->nodes[position+1];

	int middleIndex = (int) (maxOrder/2);
	
	int index = 0;
	while (index < middleIndex) {
		newNode->values[index] = oldNode->values[index + middleIndex + 1];

		newNode->nodes[index] = oldNode->nodes[index + middleIndex + 1];
		index++;
	}

	newNode->nodes[index] = oldNode->nodes[index + middleIndex + 1];
	oldNode->nodes[index + middleIndex + 1] = NULL;

	setNumKeys(newNode, index);
	setNumKeys(oldNode, numKeys(oldNode) - index - 1);

	setNumKeys(node, (size + 1));
}


// Special case when overflowing the root element
void keepOnlyMiddleElementInRoot(typeNode * node) {
	typeNode * newNode = createNode(-1);

	int position = 0;
	int middleIndex = (int) (numKeys(node) / 2);

	insertInLeave(newNode, node->values[middleIndex]);


	treeHeight++;
	// IF the whole tree was a leave until then
	if (treeHeight == 2) {
		typeNode * subNode = newNode->nodes[0];
		while (position < middleIndex) {
			insertInLeave(subNode, node->values[position]);
			position++;
		}

		position = middleIndex + 1;
		subNode = newNode->nodes[1];
		while (position <= maxOrder) {
			insertInLeave(subNode, node->values[position]);
			position++;
		}

	} else {
		typeNode * leftNode = newNode->nodes[0];

		typeNode * rightNode = newNode->nodes[1];

		while (position < middleIndex) {
			leftNode->values[position] = node->values[position];
			leftNode->nodes[position] = node->nodes[position];
			position++;
		}
		leftNode->nodes[position] = node->nodes[position];
		leftNode->numKeys[0] = middleIndex;

		position = 0;
		while (position <= maxOrder) {
			rightNode->values[position] = node->values[position+middleIndex+1];
			rightNode->nodes[position] = node->nodes[position+middleIndex+1];
			position++;
		}
		rightNode->nodes[position] = node->nodes[position+middleIndex+1];
		rightNode->numKeys[0] = middleIndex;

	}

	root = newNode;

}


// Promotes middle element and subdivide his elements
void promoteMiddleElement(typeNode * node, typeNode * parent, int height) {
	if (isRoot(parent)) {
		keepOnlyMiddleElementInRoot(node);

	} else {
		int middleIndex = (int) (numKeys(node) / 2);

		insertInNonLeave(parent, node->values[middleIndex], height);
	}
}

// Main recursive insertion method
void insertInNode(typeNode * node, int key, typeNode * parent, int height) {
	if (isLeave(height)) {
		insertInLeave(node, key);

		if (haveOverflow(node)) {
			promoteMiddleElement(node, parent, height);
		}

	} else {
		int position = findKeyInsertPosition(node, key);
		insertInNode(node->nodes[position], key, node, height + 1);

		if (haveOverflow(node)) {
			promoteMiddleElement(node, parent, height);
		}
	}
}

// Process user key insert
void commandInsert(int key) {
	if (!keyIsOnTree(root, key)) {
		insertInNode(root, key, NULL, 1);
	}
}

//
// AUXILIAR REMOTION METHODS 
//

// Redistribute nodes to be with average the same size after a remotion
typeNode * redistributeNodes(typeNode * leftNode, typeNode * rightNode, int height) {
	typeNode * newNode = createNode(-1);
	
	int leftNumKeys = numKeys(leftNode);
	int rightNumKeys = numKeys(rightNode);

	int sumNumKeys = leftNumKeys + rightNumKeys;
	
	if (sumNumKeys <= maxOrder) {
		if (isLeave(height)) {
			int position = 0;
			while (position < leftNumKeys) {
				insertInLeave(newNode, leftNode->values[position]);
			}

			position = 0;
			while (position < rightNumKeys) {
				insertInLeave(newNode, rightNode->values[position]);
			}
		}
	}

	return newNode;
}

// Try to get a replacement to when deletes a top node value, balancing his inner nodes
int getReplacementAndBalanceNodes(typeNode * leftNode, typeNode * rightNode, int height) {
	int replacementKey = -1;

	int leftNumKeys = numKeys(leftNode);
	int rightNumKeys = numKeys(rightNode);

	if (leftNumKeys > rightNumKeys) {
		replacementKey = leftNode->values[leftNumKeys-1];

		typeNode * mergedNode = redistributeNodes(
			leftNode->nodes[leftNumKeys-1], leftNode->nodes[leftNumKeys], height);

		leftNode->nodes[leftNumKeys-1] = mergedNode;

		setNumKeys(leftNode, leftNumKeys-1);
	} else {
		replacementKey = rightNode->values[0];

		typeNode * mergedNode = redistributeNodes(
			rightNode->nodes[0], rightNode->nodes[1], height);

		rightNode->nodes[1] = mergedNode;

		shiftLeftPositions(rightNode, 0, rightNumKeys);

		setNumKeys(rightNode, rightNumKeys-1);
	}

	return replacementKey;
}

//
// START REMOTION METHODS
//
void removeUnitaryValue(typeNode * leave) {
	destroyNode(leave->nodes[0]);
	destroyNode(leave->nodes[1]);

	leave->values[0] = (int) NULL;
}

void removeFromLeaveLastPosition(typeNode * leave) {
	int lastIndex = numKeys(leave);

	destroyNode(leave->nodes[lastIndex]);

	leave->values[lastIndex-1] = (int) NULL;
}

void removeFromLeavePosition(typeNode * leave, int index) {
	int lastIndex = numKeys(leave);

	leave->values[index] = (int) NULL;

	destroyNode(leave->nodes[index]);

	shiftLeftPositions(leave, index, lastIndex);
}

// Checks how it's the best way to remove from a given leave and do it
void removeFromLeave(typeNode * leave, int key) {
	int size = numKeys(leave);

	if (size == 1) {
		removeUnitaryValue(leave);

	} else {
		int position = findKeyPosition(leave, key);

		if (position == -1) {
			// avoid over decreasing size
			size = size + 1;

		} else if (position == size - 1) {
			removeFromLeaveLastPosition(leave);

		} else {
			removeFromLeavePosition(leave, position);
		}
	}

	setNumKeys(leave, (size - 1));
}

// Remove a key from a non leave and rebalance the tree
void removeFromNonLeave(typeNode * node, int index, int height) {
	typeNode * leftNode = node->nodes[index];
	typeNode * rightNode = node->nodes[index+1];

	int replacementKey = getReplacementAndBalanceNodes(leftNode, rightNode, height+1);
	node->values[index] = replacementKey;
}

// Main recursive remotion method
void removeFromNode(typeNode * node, int key, typeNode * parent, int height) {
	if (isLeave(height)) {
		removeFromLeave(node, key);
	} else {
		int position = findKeyPosition(node, key);
		if (position != -1) {
			removeFromNonLeave(node, position, height);

		} else {
			int position = findKeyInsertPosition(node, key);
			removeFromNode(node->nodes[position], key, node, height + 1);
		}

	}

}

// Process user key remove
void commandRemove(int key) {
	removeFromNode(root, key, NULL, 1);
}


//
// START PRINT METHODS
//

// print the values as height sets to debug
void printNode(typeNode * node, int height) {
	int position = 0;
	int numValues = numKeys(node);

	printf("Keys - height: %d\n",height);
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
			while (position <= numValues) {
				if (node->nodes[position]->numKeys[0] > 0) {
					printNode(node->nodes[position], height+1);
				}
				position++;
			}

			if (position > 0) {
				printf("\n");
			}
		}
	}
}

// print the values as a ordered list
void printOrdered(typeNode * node) {
	int size = numKeys(node);
	int position = 0;

	if (size > 0) {
		while (position < size) {
			printOrdered(node->nodes[position]);
			if (node->values[position] != (int) NULL) {
				printf("%d ", node->values[position]);
			}
			
			position++;
		}
		printOrdered(node->nodes[size]);
	}
}

// Process user key print
void commandPrint() {
	if (flagDebug) {
		printNode(root, 1);
	}

	printOrdered(root);
	printf("\n");
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

	while (scanf("%s", operation) != EOF) {
		if (strcmp("fim", operation) == 0) {
			break;

		} else if (strcmp("insere", operation) == 0) {
			scanf("%d", &key);
			commandInsert(key);

		} else if (strcmp("remove", operation) == 0) {
			scanf("%d", &key);
			commandRemove(key);

		} else if (strcmp("imprime", operation) == 0) {
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
