#include <stdio.h>
#include "tree.h"
#include "ST.h"
#include "y.tab.h"
#include "code.h"
#include "instr.h"

#define INT_BOOL_SIZE = 2;

extern char tokName[][12]; // token names
int DC = 0;
int LCHold = 0;
STEntry *symStack[100];
int tops = -1;

static void gen_expr (tree t)
{
	int n;
	if (t == NULL) {
		fprintf (stderr, "Internal error 11 (null expr)\n");
	}
	switch (t->kind) {

		case Plus: case Minus:
		case Star: case Slash: 
		case Mod: 

			gen_expr (t->first);
			gen_expr (t->second);
			if (t->kind == Plus) {
				code (ADDI);
			} else if (t->kind == Minus) {
				code (SUBI);
			} else if (t->kind == Star) {
				code (MULI);
			} else if (t->kind == Slash) {
				code (DIVI);
			} else if (t->kind == Mod) {
				code (DUPL); // duplicate two moded nums on stack
				code (DIVI); // mod operation
				code (MULI);
				code (SUBI);
			}
			break;
		
		case Equal: case DivEq:
		case Less: case LessEq:
		case Greater: case GreaterEq:

			gen_expr (t->first);
			gen_expr (t->second);
			if (t->kind == Equal) {
				code (SUBI);
				code (TSTEQI);
			} else if (t->kind == DivEq) {
				code (SUBI);
				code (TSTEQI);
				code (NOTW);
			} else if (t->kind == Less) {
				code (SUBI);
				code (TSTLTI);
			} else if (t->kind == LessEq) {
				code (SWAPW);
				code (SUBI);
				code (TSTLTI);	
				code (NOTW);
			} else if (t->kind == Greater) {
				code (SWAPW);
				code (SUBI);
				code (TSTLTI);
			} else if (t->kind == GreaterEq) {
				code (SUBI);
				code (TSTLTI);
				code (NOTW);
			}	
			break;
		
		case Or: case And: case Xor: case Not:
			gen_expr (t->first);
			gen_expr (t->second);
			if (t->kind == Or) {
				code (ORW);
			} else if (t->kind == And) {
				code (ANDW);	
			} else if (t->kind == Xor) {
				code (XORW);
			} else if (t->kind == Not) {
				code (NOTW);
			}		
			break;
		
		case Range:
			gen_expr (t->first);
			break;
					
		case LBrac: //check index w/ declared range?
			code1 (PUSHW, ST[t->first->value]->addr); 
			gen_expr (t->second);

			// test if reference in array lower bound
			code (DUPW);
			code1 (PUSHW, ST[t->first->value]->aStart); // lower array bound
			code (SUBI);
			code (TSTLTI);
			code1 (RGOZ, 1); // what is the 1 actually??
			code (HALT);
			// test if reference in array upper bound
			code (DUPW);
			code1 (PUSHW, ST[t->first->value]->aEnd); // upper array bound
			code (SWAPW);
			code (SUBI);
			code (TSTLTI);
			code1 (RGOZ, 1); // what is the 1 actually??
			code (HALT);	
			 
			code1 (PUSHW, sizeof(int)); // should be size of base type 
			code (MULI);
			code (ADDI);
			code (GETSW);
			break;

		case IntConst: case True: case False: // should True & False be here?
			if (t->kind == True) {
				code1 (PUSHW, -1);
			} else {
				code1 (PUSHW, t->value);
			}
			break;
			
		case Ident:	
			code1 (PUSHW, ST[t->value]->addr);
			code (GETSW);
			break;
				
		default:
		//	fprintf (stderr, "Internal error 12\n");	
			fprintf (stderr, "expr fell through on %s\n", tokName[t->kind]);
	}
}		

void gen_stmt (tree t) 
{
	for (; t != NULL; t = t->next) {
		switch (t->kind) {
			case Integer: case Boolean: case Array:
				/*do nothing*/
				break;

			case Colon:
				LCHold = LC;
				LC = DC;

				prLC();
				ST[t->first->value]->addr = LC;
				int start, end;
				if (t->second->kind == Integer) {
					ST[t->first->value]->typeSize = 2;//size of array
				} else if (t->second->kind == Boolean) {
					ST[t->first->value]->typeSize = 2;//size of array
				} else if (t->second->kind == Array) {
					ST[t->first->value]->aEnd = t->second->first->second->value;
					ST[t->first->value]->aStart = t->second->first->first->value;
					end = ST[t->first->value]->aEnd;
					start = ST[t->first->value]->aStart;
					ST[t->first->value]->typeSize = (end-start+1)*2;//size of array
					// arrays must be boolean or integer so hard code 2
				}		
				LC += ST[t->first->value]->typeSize;

				tree tmp = t->first;
				while ((tmp = tmp->next) != NULL) {// id_list
					prLC();
					ST[tmp->value]->addr = LC;
					if (ST[tmp->value]->type == Integer) {
						ST[tmp->value]->typeSize = 2;
					} else if (ST[tmp->value]->type == Boolean) {
						ST[tmp->value]->typeSize = 2;
					} else if (ST[tmp->value]->type == Array) {
						// needs to use range values for that id_list
						// saved in old end, start
						//end = ST[tmp->value]->aEnd;
						//start = ST[tmp->value]->aStart;
						ST[tmp->value]->typeSize = (end-start+1)*2;		
					}
					LC += ST[tmp->value]->typeSize;
				}	
				//gen_expr (t->first);
				//gen_stmt (t->second); 
				prNL();
				DC = LC;
				LC = LCHold;
				break;

			case Assign:
				prLC();
				if (t->first->kind == Ident) { // normal assignment
					code1 (PUSHW, ST[t->first->value]->addr);
				} else { // array ref assignment	
					code1 (PUSHW, ST[t->first->first->value]->addr);
				}
				gen_expr (t->second);
				code (PUTSW);
				prNL();
				break;
			
			case If: case Elsif: {
				struct FR fix1, fix2;
				prLC();
				gen_expr (t->first);
				fix1 = codeFR (RGOZ);
				prNL();
				gen_stmt (t->second);
				if (t->third != NULL) {
					prNL();
					prLC();
					fix2 = codeFR (RGOTO);
					fixFR (fix1, LC);
					gen_stmt (t->third);
					fixFR (fix2, LC);
				} else {
					fixFR (fix1, LC);
				}	
			}	
			break;

			case Else:
				//prLC();
				gen_stmt (t->first);
				prNL();
				break;

			case For: { 
				struct FR fix1, fix2; 
				int place, oldTypeSize;
				bool isReDecl = false;

				LCHold = LC;
				LC = DC;

				pr_directive(".DATA");
				prLC();
				ST[t->first->value]->addr = LC;
				// For loop will always be integer
				if (ST[t->first->value]->typeSize == -1) {
					ST[t->first->value]->typeSize = 2;
				} else {
					oldTypeSize = ST[t->first->value]->typeSize;
					ST[t->first->value]->typeSize = 2;
					isReDecl = true;
				}	
				LC += 2;

				DC = LC;
				LC = LCHold;
				prNL();
					
				pr_directive (".CODE");
				/*gen_expr (t->second); // lower range not needed?
				code (DUPW);
				prNL();*/

				prLC();
				gen_expr (t->second->second); // upper range 
				code (DUPW);
				prNL();
				
				// Ident == upper expr -> the test
				place = LC;
				prLC();
				code1 (PUSHW, ST[t->first->value]->addr);
				code (GETSW);
				code (SUBI);
				code (TSTEQI);
				fix1 = codeFR (RGOZ); // if 0 (false), loop done

				gen_stmt (t->third); // check For loop body
				// increment index Ident
				code1 (PUSHW, ST[t->first->value]->addr); // push index addr 
				code (DUPW);
				code (GETSW);
				code1 (PUSHW, 1); 
				code (ADDI);
				code (PUTSW);
				prNL();

				prLC();
				code (DUPW);
				code1 (RGOTO, place); // return to test??
				fixFR (fix1, LC);
				prNL();

				prLC();
				code1 (PUSHW, -1);
				code (CHSPS);
				prNL();

				// fix possible redecl
				if (isReDecl) {
					ST[t->first->value]->typeSize = oldTypeSize;
				}
				break;
			}
		
		case Declare: 
			pr_directive (".DATA");
			//prNL();
			// save old sym table entries
			if (ST[t->first->first->value]->addr != -1) {
				tops++;
				symStack[tops] = malloc (sizeof (STEntry*));	
				symStack[tops]->addr = ST[t->first->first->value]->addr;
				symStack[tops]->typeSize = ST[t->first->first->value]->typeSize;
				symStack[tops]->index = ST[t->first->first->value]->index;
				tree tmp = NULL;
				if (t->first->first->next != NULL) {
					tmp = t->first->first->next;
				}
				while (tmp != NULL) { // other decls in id_list 
					if (ST[tmp->value] != NULL && ST[tmp->value]->addr != -1) {
						tops++;
						symStack[tops] = malloc (sizeof (STEntry*));	
						symStack[tops]->addr = ST[t->value]->addr;
						symStack[tops]->typeSize = ST[t->value]->typeSize;
						symStack[tops]->index = ST[t->value]->index;
					}	
				}	
			}
			gen_stmt (t->first); // send Colon for decls
			pr_directive (".CODE");
			//prNL();
			gen_stmt (t->second); // check stmts
			prNL();
			while (tops != -1) {
				ST[symStack[tops]->index]->addr = symStack[tops]->addr;
				ST[symStack[tops]->index]->typeSize = symStack[tops]->typeSize;
				tops--;	
			}	
			break;

		case Exit: { 
			struct FR fix4;
			if (t->first != NULL) {
				gen_expr (t->first); // how to test expr result?
				// some boolean result on stack
				code (NOTW); // want to jump only if true so reverse it
				fix4 = codeFR (RGOZ);	// FR to jump to "end loop"
			} else {
				fix4 = codeFR (RGOTO); // FR to jump to "end loop"
			}	
			break;
		}
				
		default:
			fprintf (stderr, "Stmt fell through on %s\n", tokName[t->kind]);	
			
		}
	}

}

void gen_program (tree t)
{
	LC = LCHold;
	LC = DC;
	pr_directive (".DATA");
	gen_stmt (t->second); // decls

	LC = LCHold;
	pr_directive (".CODE");
	gen_stmt (t->third); // body
	LCHold = LC;

	prLC();
	code (HALT);
	pr_directive (".ENTRY 0");
	prNL();
}

	
