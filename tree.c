#include "tree.h"
#include "y.tab.h"

tree buildTree (int kind, tree one, tree two, tree three) {
	tree t = (tree) malloc(sizeof(node));       
	t->kind = kind;
	t->first = one;
	t->second = two;
	t->third = three;
	t->next = NULL;
	
	return t; 
}

tree buildIntTree (int kind, int val) {
	tree t = (tree) malloc(sizeof(node));
	t->kind = kind;
	t->value = val;
	t->first = t->second = t->third = NULL;
	t->next = NULL;

	return t;
}

char tokName[][12] = 
	{"", "BOOLEAN", "INTEGER", "TRUE", "FALSE", "", "", "", "", "",
	"and", "array", "begin", "declare", "else", "elsif", "end", "exit", "for", "if",
	"in", "is", "loop", "mod", "not", "on", "of", "or", "procedure", "then", "when",
	"while", "xor", "", "", "", "", "", "", "",
	"Ident", "IntConst", "", "", "", "", "", "", "", "",
	"=", "/=", "<=", ">=", "<", ">", "+", "-", "*", "/",
	"(", ")", "[", "]", ":=", "..", ";", ":", ",", "NoType"};
static int indent = 0;
void printTree (tree t) {
	if (t == NULL) return;
	for (; t != NULL; t = t->next) {
		printf ("%*s%s", indent, "", tokName[t->kind]);
		switch (t->kind) {
			case Ident:
				printf ("		%s (%d)\n", id_name(t->value), t->value);
				break;
			case IntConst:
				printf ("		%d\n", t->value);
				break;
			default:
				printf ("\n");
				indent += 2;
				printTree (t->first);
				printTree (t->second);
				printTree (t->third);
				indent -= 2;
		} // end switch
	} // end for loop
} // end printTree()




