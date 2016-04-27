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
				code (MULTI);
			} else if (t->kind == Slash) {
				code (DIVI);
			}			
			break;
		
		case Equals: case DivEq:
		case Less: case LessEq:
		case Greater: case GreaterEq:

			gen_expr (t->first);
			gen_expr (t->second);
			if (t->kind == Equals) {
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
		
		case IntCost:
			code1 (PUSHW, t->value);
			break;
		
		case Ident:	
			code1 (PUSHW, addr_of (t));
			code (GETSW);
			break;

		default:
			fprintf (stderr, "Internal error 12\n");	
	}
}		

static void gen_stmt (tree t) 
{
	for (; t != NULL; t = t->next) {
		switch (t->kind) {
			case Integer: case Boolean: // case Array:??
				/* do nothing */
				break;

			case Assign:
				prLC();
				code1 (PUSHW, addr_of (t->first));
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

	
