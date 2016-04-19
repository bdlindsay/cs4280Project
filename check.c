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
int scope = 0; // no scope at beginning
int tos = -1; // no stack at beginning
int error_num = 0;

// prototype
void type_error(int);
extern char* id_name(int);

void printST () {
	int i;
	char type[9] = "<none>  ";
	printf("SYMBOL TABLE - scope: %d\n", scope);
	for (i = 0; i <= top; i++) {
		//fprintf (stderr, "its scope: %d the scope: %d\n", ST[i]->scope, scope);
		if (ST[i]->scope <= scope) { // only print if visible in current scope
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
				printf ("%3d %-10s\t%-16s scope: %d\n", i,id_name (i), type, ST[i]->scope);
			} else {
				printf ("%3d %-10s\t%-16s scope: %d range: %d..%d\n", 
					i, id_name (i), type, ST[i]->scope, ST[i]->aStart, ST[i]->aEnd); 
			}
		}
	}
}

int check_expr (tree t) {
	int typeL, typeR, typeI;
	if (t == NULL) {
		fprintf (stderr, "Shouldn't be here: missing expression\n");
		return NoType;
	}

	switch (t->kind) { // switch for expr type-checking

		case Plus: case Minus: case Star: case Slash: case Mod:
			// arithmetic operators are Integers and return Integer
			typeL = check_expr (t->first);
			if (t->second != NULL) { // binary operator
				typeR = check_expr (t->first);
				if (typeL == Integer && typeR == Integer) {
					return Integer;
				} else {
					type_error(t->kind); 
					return NoType;
				}
			} else { // unary operator, only plus/minus
				if (typeL == Integer && t->kind == Plus || t->kind == Minus) {
					return Integer;
				} else {
					type_error(t->kind);
					return NoType;
				}
			}
			break;
		
		case Equal: case DivEq: 
		case Less: case LessEq: 
		case Greater: case GreaterEq:
			// relational operators must agree, result bool
			typeL = check_expr (t->first);
			typeR = check_expr (t->second);
			if (typeL == typeR && typeL == Integer || typeL == Boolean) { 
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
				type_error(t->kind); 
				return NoType;
			}
			break;	
		
		case IntConst:
			return Integer;

		case Boolean:
			return Boolean;
		
		case True: case False:
			return Boolean;

		case Ident:
			// ST lookup
			if ( ST[t->value]->valid == false || ST[t->value]->scope > scope) {
				fprintf(stderr, "Entry %d-%s invalid in ST:%d\n", t->value, id_name (t->value), scope);
				error_num++;
				return NoType;
			} else {
				return ST[t->value]->type;
			}

		case LBrac: // Ident[index]
			// check that index is type Integer
			typeI = check_expr(t->second); 
			if ( typeI != Integer) {
				type_error(t->kind);
				return NoType;
			} else {
				// ST lookup		
				if ( ST[t->first->value]->valid == false || ST[t->first->value]->type != Array || 
							ST[t->first->value]->scope > scope) {
					fprintf(stderr, "Entry %d-%s invalid in ST:%d or not an Array\n", t->value, id_name (t->value), scope);
					error_num++;
					return NoType;
				}	else {
					return ST[t->first->value]->arrayBaseT;
				}		
			}
			break;

		default: 
			fprintf (stderr, "You shouldn't be here; invalid expression operator %d %s\n", 
				t->kind, tokName[t->kind]);

	} // end switch(t->kind)

} // end check_expr()

static void handle_decls (tree t) {
	int i;
	if (scope == 1) {
		for ( i = 0; i < 100; i++) {
			ST[i] = (STEntry *) malloc( sizeof( STEntry* )); 
		}
		// init the NoType entry of ST
		ST[0]->index = 0;
		ST[0]->type = NoType;
	}
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
			static int test = 0;
			if (pos == 2)
				test++;
			if (ST[pos]->valid == true || test == 2) { // don't overwrite existing
				push (ST[pos]); // push possible prev scope entry to stack
				ST[pos] = (STEntry *) malloc( sizeof( STEntry*)); // new entry here
			}
			ST[pos]->index = pos;
			ST[pos]->type = type;
			ST[pos]->scope = scope;
			char *tmp = id_name (p->value);
			ST[pos]->name = tmp;
			ST[pos]->valid = true;	
			if (type == Array) {
				ST[pos]->aStart = t->second->first->first->value;
				ST[pos]->aEnd = t->second->first->second->value;
				if (t->second->second->kind != Integer) {
					type_error(t->second->second->kind);
				}
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
				enterScope();	
				fprintf (stderr, "Procedure: scope++ : %d\n", scope);
				handle_decls (t->second);

				check_stmts (t->third);

				printST();
				exitScope();
				fprintf (stderr, "Procedure: scope-- : %d\n", scope);
				break;	

			case Assign:
				if ( t->first->kind != LBrac) { // non-array assignment
					// ST lookup
					if ( ST[t->first->value]->valid && ST[t->first->value]->scope <= scope) { // entry in ST
						typeL = ST[t->first->value]->type; 
						typeR = check_expr(t->second);
					} else { // no visible entry in ST
						fprintf(stderr, "Entry %d-%s invalid in ST:%d\n", t->first->value, id_name (t->first->value), scope);
						error_num++;
						return;
					}	
				} else if (t->first->kind == LBrac) { // array assignment
					// ST array lookup
					if ( ST[t->first->first->value]->valid && ST[t->first->first->value]->type == Array &&
							ST[t->first->first->value]->scope <= scope) { // entry in ST
						typeL = check_expr (t->first);
						typeR = check_expr (t->second);	
					} else { // no visible entry in ST
						fprintf(stderr, "Entry %d-%s invalid in ST:%d\n", 
							t->first->first->value, id_name (t->first->first->value), scope);
						error_num++;
						return;
					}
				}
				if (typeL != typeR) { // LHS and RHS of assignment must match
					type_error(t->kind);
					return;
				}
				break;
			
			case If: case Elsif:
				// expr in If must be Boolean
				if (check_expr(t->first) != Boolean) {
					type_error(t->kind);
					return;	
				}
				check_stmts(t->second);
				check_stmts(t->third);
				break;
			
			case Else:
				check_stmts(t->first);	
				break;

			case For:
				// starts a new scope
				enterScope();
				fprintf (stderr, "For: scope++ : %d\n", scope);
				// 2 range values must be same type
				t_start = check_expr(t->second->first);
				t_end = check_expr(t->second->second);
				if ( t_start != t_end || t_start == NoType) {
					type_error(t->kind);
					return;
				} else {
					// add Ident to ST w/ type of t_start in a new scope
					int pos = t->first->value; // Ident in For loop
					if ( ST[pos]->valid == true) {
						push (ST[pos]); // push prev scope entry to stack
					}
					ST[pos]->index = pos;
					ST[pos]->type = t_start;
					ST[pos]->scope = scope; // will be scope not 2
					char *tmp = id_name(pos);
					ST[pos]->name = tmp;
					ST[pos]->valid = true;	
				}

				check_stmts(t->third); // body of For loop

				printST();
				exitScope();
				fprintf (stderr, "For: scope-- : %d\n", scope);
				break;

			// Exit without bool_expr handled in for loop b/c tree will be NULL
			case Exit:
				typeL = check_expr(t->first);
				if ( typeL != Boolean || typeL == NoType) {
					type_error( t->kind);
				}
				break;
			
			case Declare:
				// starts a new scope
				enterScope();
				fprintf (stderr, "Declare: scope++ : %d\n", scope);
				handle_decls (t->first);

				check_stmts (t->second);		

				printST();
				exitScope();
				fprintf (stderr, "Declare: scope-- : %d\n", scope);
				break;
	
			default:
				fprintf(stderr, "No stmt match for token %d\n", t->kind);
	
				// endScope()
		} // end switch statement

	}	// end for loop

} // end check_stmts()

void enterScope () {
	symStack[++tos] = (STEntry *) -1; // push mark
	scope++;
	if (scope == 1) { // do nothing for first scope
		return;
	}
}

void push (STEntry * entry) {
	fprintf (stderr, "Pushing %s to stack for re-decl in scope %d.\n", entry->name, scope);
	if ( entry->scope == scope) { // pushing old scope entry to make room for new entry
		fprintf (stderr, "Scope error on pos: %d\n", entry->index); // scopes should be different
	} else {
		symStack[++tos] = entry;
	} 	
}

void exitScope () {
	for (; symStack[tos] != (STEntry*) -1; tos--) {
		ST[symStack[tos]->index] = symStack[tos]; // pop value into ST		
	}
	tos--; // pop (STEntry*) -1	
	scope--;
}

void type_error(int kind) {
	fprintf(stderr, "Error: Type mismatch for %s\n", tokName[kind]); 
	error_num++;
}

			
