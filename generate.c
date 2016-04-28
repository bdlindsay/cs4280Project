#include <stdio.h>
#include "tree.h"
#include "y.tab.h"
#include "code.h"
#include "instr.h"

#define addr_of(t) (2 * (t->value -1))

static void gen_expr (tree t)
{
	int n;
	if (t == NULL) {
		fprintf (stderr, "Internal error 11\n");
	}
	switch (t->kind) {

		case Plus: case Minus:
		case Star: case Slash: 

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
		
		case IntConst: case True: case False: // should True & False be here?
			code1 (PUSHW, t->value);
			break;
			
		case Ident:	
			code1 (RPUSHA, addr_of (t));
			code (GETSW);
			break;

		case Array: // this or LBrac, check index w/ declared range
			code1 (RPUSHA, addr_of (t->first)); 
			gen_expr (t->second);
			code1 (PUSHW, sizeof(IntConst)); // base type??
			code (MULI);
			code (ADDI);
			code (GETSW);
			break;
				
		default:
			fprintf (stderr, "Internal error 12\n");	
	}
}		

void gen_stmt (tree t) 
{
	for (; t != NULL; t = t->next) {
		switch (t->kind) {
			case Integer: case Boolean: // case Array:??
				/* do nothing */
				break;

			case Assign:
				prLC();
				code1 (RPUSHA, addr_of (t->first));
				gen_stmt (t->second);
				code (PUTSW);
				prNL();
				break;
			
			case If: {
				struct FR fix1, fix2;
				prLC();
				gen_expr (t->first);
				fix1 = codeFR (RGOZ);
				prNL();
				gen_stmt (t->second);
				if (t->third != NULL) {
					fix2 = codeFR (RGOTO);
					fixFR (fix1, LC);
					gen_stmt (t->third);
					fixFR (fix2, LC);
				} else {
					fixFR (fix1, LC);
				}	
			}	
			break;

			case For: { // TODO handle Exit [when]
				struct FR fix1, fix2, fix3; // fix 3 for exit?
				prLC();
				code1 (RPUSHA, addr_of (t->first));
				gen_expr (t->second->first); // lower range
				code (PUTSW); // is this correct?
				gen_expr (t->second->second); // upper range

				// first <= second
				code1 (PUSHW, t->second->first->value); // needed?
				code1 (PUSHW, t->second->second->value); // needed?
				code (SWAPW);
				code (SUBI);
				code (TSTLTI);	
				code (NOTW);
				fix1 = codeFR(RGOZ); // if 0 (false), error  
				
				fix2 = codeFR (RGOTO); // return to test??
				// Ident <= upper expr
				code1 (PUSHW, t->first->value);
				code1 (PUSHW, t->second->second->value);
				code (SWAPW);
				code (SUBI);
				code (TSTLTI);	
				code (NOTW);
				fix3 = codeFR (RGOZ); // if 0 (false), loop done

				gen_stmt (t->third);
				code1 (PUSHW, 1); // push 1 to stack?
				code1 (PUSHW, t->first->value); // push index val?
				code (ADDI); // increment?
				code1 (RGOTO, fix2.LChere); // how to do this??
				fixFR (fix1, LC);
				fixFR (fix3, LC);
				break;
			}
		}
	}

}

void gen_program (tree t)
{
	pr_directive (".CODE .ENTRY 0");
	gen_stmt (t->second);
	prLC();
	code (HALT);
	prNL();
}

	
