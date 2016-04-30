#include <stdio.h>
#include "tree.h"
#include "y.tab.h"
#include "code.h"
#include "instr.h"

#define addr_of(t) (2 * (t->value -1))
extern char tokName[][12]; // token names
int dataLC = 0;
int codeLC = 0; 

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
				fprintf (stderr, "Found Mod - holding off\n");	
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
			code (PUTSW);
			//gen_expr (t->second); // upper range
			//code (DUPW);
			break;
					
		case LBrac: //check index w/ declared range?
			code1 (PUSHW, addr_of (t->first)); 
			gen_expr (t->second);
			code1 (PUSHW, sizeof(IntConst)); // base type??
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
			code1 (PUSHW, addr_of (t)); 
			code (GETSW);
			// additional decls
			if (t->next != NULL) {
				prLC();
				gen_expr (t->next);
			}	
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
			case Integer: case Boolean: case Array: // should Array be here too?
				/* do nothing */
				break;

			case Colon:
				prLC();
				gen_expr (t->first);
				gen_stmt (t->second);
				prNL();
				break;

			case Assign:
				prLC();
				code1 (PUSHW, addr_of (t->first));
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
				int place;
				prLC();
				code1 (PUSHW, addr_of (t->first)); 
				gen_expr (t->second); // lower range
				prNL();

				codeLC = LC;
				LC = dataLC;
				pr_directive(".DATA");
				prLC();
				gen_expr (t->first);
				prNL();

				dataLC = LC;
				LC = codeLC;
				pr_directive(".CODE");
				prLC();
				gen_expr (t->second->second); // 
				code (DUPW);
				prNL();
				
				// Ident == upper expr -> the test
				place = LC;
				prLC();
				code1 (PUSHW, addr_of (t->first));
				code (GETSW);
				code (SUBI);
				code (TSTEQI);
				fix1 = codeFR (RGOZ); // if 0 (false), loop done

				gen_stmt (t->third); // check For loop body
				// increment index Ident
				code1 (PUSHW, addr_of (t->first)); // push index addr 
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
				break;
			}
		
		case Declare: 
			prLC();
			gen_stmt (t->first); // send Colon for decls?
			gen_stmt (t->second); // check stmts
			prNL();
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
	LC = dataLC;
	pr_directive (".DATA");
	gen_stmt (t->second); // decls
	dataLC = LC;

	LC = codeLC; // is this how you do this?
	pr_directive (".CODE");
	gen_stmt (t->third); // body
	codeLC = LC;

	prLC();
	code (HALT);
	pr_directive (".ENTRY 0");
	prNL();
}

	
