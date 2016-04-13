#include <stdio.h>
#include "ST.h"
#include "tree.h"
#include "y.tab.h"
#include <string.h>

// Brett Lindsay 
// Context Checker
// cs4280 Part 3

extern int top;
extern char tokName[][12];
STEntry *ST[100];
STEntry *symStack[300];
int error_num = 0;

// prototype
void type_error(int);

void printST () {
	int i;
	char type[9] = "<none>  ";
	printf("SYMBOL TABLE\n");
	for (i = 0; i <= top; i++) {
		int t = ST[i]->type;
		if (t == Boolean) {
			strcpy (type, "Boolean");
		} else if ( t == Integer) {
			strcpy (type, "Integer");
		} else if (t == Array && ST[i]->arrayBaseT == Integer) {
			strcpy (type, "Array:Integers");
		} else if (t == Array && ST[i]->arrayBaseT == Boolean) {
			strcpy (type, "Array:Booleans");
		} else {
			strcpy (type, "<none>  ");
		}
		if (t != Array) {
			printf ("%3d %-10s\t%s\n", i, id_name (i), type);
		} else {
			printf ("%3d %-10s\t%s\trange: %d..%d\n", 
				i, id_name (i), type, ST[i]->aStart, ST[i]->aEnd); 
		}
	}
}

int check_expr (tree t) {
	int typeL, typeR;
	if (t == NULL) {
		fprintf (stderr, "Shouldn't be here: missing expression\n");
		return NoType;
	}

	switch (t->kind) { // switch for expr type-checking
		// Declare starts a new scope that ends at corresponding End
		// TODO
		
		// TODO deal with unary plus/minus, might need .y change	
		case Plus: case Minus: case Star: case Slash: case Mod:
			// arithmetic operators are Integer and return Integer
			typeL = check_expr (t->first);
			typeR = check_expr (t->second);
			if (typeL == Integer && typeR == Integer) {
				return Integer;
			} else {
				type_error(t->kind); 
				return NoType;
			}
			break;
		
		case Equal: case DivEq: 
		case Less: case LessEq: 
		case Greater: case GreaterEq:
			// relational operators must agree, result bool
			typeL = check_expr (t->first);
			typeR = check_expr (t->second);
			if (typeL == typeR) { 
				return Boolean;
			} else {
				type_error(t->kind);
				return NoType;
			}
			break;
	
		case Or: case And: case Xor: case Not:
			// operands for above must be boolean and result boolean
			typeL = check_expr (t->first);
			typeR = check_expr (t->second);
			if (typeL == Boolean && typeR == Boolean) {
				return Boolean;
			}	else {
				type_error(t->kind); return NoType;
			}
			break;	
		// type check array variables
		
		case IntConst:
			return Integer;

		case Boolean:
			return Boolean;
		
		case True: case False:
			return Boolean;

		case Ident:
			// ST lookup
			return ST[t->value]->type;

		case LBrac: // Ident[index]
			// ST lookup		
			break;

		default: 
			fprintf (stderr, "You shouldn't be here; invalid expression operator\n");
	} // end switch(t->kind)

} // check_expr

static void handle_decls (tree t) {
	int i;
	fprintf(stderr, "Handling decls/Creating Sym Table.\n");
	for ( i = 0; i < 100; i++) {
		ST[i] = (STEntry *) malloc( sizeof( STEntry* )); 
	}
	// init the NoType entry of ST
	ST[0]->index = 0;
	ST[0]->type = NoType;

	for (; t != NULL; t = t->next) {
		int type = t->second->kind; 
		tree p;
		if (type != Integer && type != Boolean && type != Array) {
			fprintf( stderr, "Bad type in decl\n"); 
			continue;
			//return;
		}
		for (p = t->first; p != NULL; p = p->next) {
			int pos = p->value;
			ST[pos]->index = pos;
			ST[pos]->type = type;
			ST[pos]->scope = 1;
			char *tmp = id_name(p->value);
			strcpy(ST[pos]->name, tmp);
			ST[pos]->valid = true;	
			if (type == Array) {
				ST[pos]->aStart = t->second->first->first->value;
				ST[pos]->aEnd = t->second->first->second->value;
				ST[pos]->arrayBaseT = t->second->second->kind;
			}
		}
	} // end for t = t->next

} // end handle_decls

void check_stmts (tree t) {
	int typeL, typeR, t_start, t_end;
	for (; t != NULL; t = t->next) {
		switch (t->kind) {
			case Procedure: 
				handle_decls (t->second);
				check_stmts (t->third);
				fprintf(stderr, "Printing ST...\n");
				printST();
				break;	
			case Assign:
				typeL = ST[t->first->value]->type; // ST type lookup of Ident t->first
				typeR = check_expr(t->second);
				if (typeL != typeR) {
					type_error(t->kind);
					return;
				}
				break;
			
			case If: case Elsif:
				if (check_expr(t->first) != Boolean) {
					type_error(t->kind);
					return;	
				}
				check_stmts(t->second);
				check_stmts(t->third);
				break;
				
			case For:
				// 2 values in range must be same
				t_start = check_expr(t->first->second);
				t_end = check_expr(t->first->third);
				if (t_start != Integer || t_end != Integer || t_start != t_end) {
					type_error(t->kind);
					return;
				} else {
					// add Ident to ST w/ type of t_start in a new scope
				}
				check_stmts(t->second); // body of For loop
				break;
			
			default:
				fprintf(stderr, "Should this be here?\n");
	
				// endScope()
		} // end switch statement

	}	// end for loop

} // end check_stmts()

void type_error(int kind) {
	fprintf(stderr, "Error: Type mismatch for %d: %s\n", kind, tokName[kind]); 
	error_num++;
}

			
