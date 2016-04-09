#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	int kind, value;
	struct Node *first, *second, *third, *next;
} node;
typedef node *tree;

extern char tokName[][12];
extern tree root;

tree buildTree (int kind, tree first, tree second, tree third);
tree buildIntTree (int kind, int val);
void printTree (tree);


#endif
