%{
#include "tree.h"

tree root = NULL;
%}


%token <t> Boolean 1 Integer 2 True 3 False 4
%token <t> And 10 Array 11 Begin 12 Declare 13 Else 14 Elsif 15 End 16 Exit 17
%token <t> For 18 If 19 In 20 Is 21 Loop 22 Mod 23 Not 24 On 25 Of 26 Or 27
%token <t> Procedure 28 Then 29 When 30 While 31 Xor 32
%token <i> Ident 40 IntConst 41
%token <t> Equal 50 DivEq 51 LessEq 52 GreaterEq 53 Less 54 Greater 55
%token <t> Plus 56 Minus 57 Star 58 Slash 59
%token <t> LPar 60 RPar 61 LBrac 62 RBrac 63
%token <t> Assign 64 Range 65 SemiColon 66 Colon 67 Comma 68 
%token NoType 69

%start program

%union { tree t; int i; }

%type <t> decls declaration id_list stmts statement ref expr  relation
%type <t> sum prod factor basic type const_range range end_if sign

%%
program
	: Procedure Ident Is decls Begin stmts End SemiColon
		{ root = buildTree(Procedure, buildIntTree(Ident, $2), $4, $6); }
	;
decls
	: /* empty */
		{ $$ = NULL; }
	| declaration SemiColon decls
		{ $1-> next = $3; $$ = $1; }
declaration
	: id_list Colon type
		{ $$ = buildTree(Colon, $1, $3, NULL); /*$1->next = $3; $$ = $1;*/ }
	;
id_list
	: Ident
		{ $$ = buildIntTree (Ident, $1); }
	| Ident Comma id_list
		{ $$ = buildIntTree (Ident, $1); $$->next = $3; }
	;
type
	: Integer
		{ $$ = buildTree(Integer, NULL, NULL, NULL); }
	| Boolean
		{ $$ = buildTree(Boolean, NULL, NULL, NULL); }
	| Array LBrac const_range RBrac Of type
		{ $$ = buildTree (Array, $3, $6, NULL); }
	;
const_range
	: IntConst Range IntConst
		{ $$ = buildTree (Range, buildIntTree(IntConst, $1),
															buildIntTree(IntConst, $3), NULL); }
	;
stmts
	: /* empty */
		{ $$ = NULL; }
	| statement SemiColon stmts
		{ $1->next = $3; $$ = $1; }
	;
statement
	: ref Assign expr
		{ $$ = buildTree (Assign, $1, $3, NULL); }
	| Declare decls Begin stmts End
		{ $$ = buildTree (Declare, $2, $4, NULL); }
	| For Ident In range Loop stmts End Loop
		{ $$ = buildTree (For, buildIntTree (Ident, $2), $4, $6); }
	| Exit
		{ $$ = buildTree (Exit, NULL, NULL, NULL); }
	| Exit When expr
		{ $$ = buildTree (Exit, $3, NULL, NULL); }
	| If expr Then stmts end_if
		{ $$ = buildTree (If, $2, $4, $5); }
	;
range
	: sum Range sum
		{ $$ = buildTree (Range, $1, $3, NULL); }
	;
ref
	: Ident
		{ $$ = buildIntTree(Ident, $1); }
	| Ident LBrac expr RBrac
		{ $$ = buildTree(LBrac, buildIntTree (Ident, $1), $3, NULL); }
	;
end_if
	: End If
		{ $$ = NULL; }
	| Else stmts End If
		{ $$ = buildTree(Else, $2, NULL, NULL); }
	| Elsif expr Then stmts end_if
		{ $$ = buildTree(Elsif, $2, $4, $5); }
	;
expr
	: relation Or relation
		{ $$ = buildTree (Or, $1, $3, NULL); }
	| relation And relation
		{ $$ = buildTree (And, $1, $3, NULL); }
	| relation Xor relation
		{ $$ = buildTree (Xor, $1, $3, NULL); }
	| relation
		{ $$ = $1; }
	;
relation
	: sum
		{ $$ = $1; }
	| sum Equal sum
		{ $$ = buildTree (Equal, $1, $3, NULL); }
	| sum DivEq sum
		{ $$ = buildTree (DivEq, $1, $3, NULL); }
	| sum Less sum
		{ $$ = buildTree (Less, $1, $3, NULL); }
	| sum LessEq sum
		{ $$ = buildTree (LessEq, $1, $3, NULL); }
	| sum Greater sum
		{ $$ = buildTree (Greater, $1, $3, NULL); }
	| sum GreaterEq sum
		{ $$ = buildTree (GreaterEq, $1, $3, NULL); }
	;
sum
	: sign prod
		{ $2->next = $1; $$ = $2; }
	| sum Plus prod
		{ $$ = buildTree (Plus, $1, $3, NULL); }
	| sum Minus prod
		{ $$ = buildTree (Minus, $1, $3, NULL); }
	;
sign
	: Plus
		{ $$ = buildTree(Plus, NULL, NULL, NULL); }
	| Minus
		{ $$ = buildTree(Minus, NULL, NULL, NULL); }
	| /* empty */
		{ $$ = NULL; }
	;
prod
	: factor
		{ $$ = $1; }
	| prod Star factor
		{ $$ = buildTree (Star, $1, $3, NULL); }
	| prod Slash factor
		{ $$ = buildTree (Slash, $1, $3, NULL); }
	| prod Mod factor
		{ $$ = buildTree (Mod, $1, $3, NULL); }
	;
factor
	: Not basic
		{ $$ = buildTree (Not, $2, NULL, NULL); }
	| basic
		{ $$ = $1; }
	;
basic
	: ref
		{ $$ = $1; }
	| LPar expr RPar
		{ $$ = $2; }
	| IntConst
		{ $$ = buildIntTree (IntConst, $1); }
	| True
		{ $$ = buildTree(True, NULL, NULL, NULL); }
	| False
		{ $$ = buildTree(False, NULL, NULL, NULL); }
	;
