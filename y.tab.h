/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     Boolean = 1,
     Integer = 2,
     True = 3,
     False = 4,
     And = 10,
     Array = 11,
     Begin = 12,
     Declare = 13,
     Else = 14,
     Elsif = 15,
     End = 16,
     Exit = 17,
     For = 18,
     If = 19,
     In = 20,
     Is = 21,
     Loop = 22,
     Mod = 23,
     Not = 24,
     On = 25,
     Of = 26,
     Or = 27,
     Procedure = 28,
     Then = 29,
     When = 30,
     While = 31,
     Xor = 32,
     Ident = 40,
     IntConst = 41,
     Equal = 50,
     DivEq = 51,
     LessEq = 52,
     GreaterEq = 53,
     Less = 54,
     Greater = 55,
     Plus = 56,
     Minus = 57,
     Star = 58,
     Slash = 59,
     LPar = 60,
     RPar = 61,
     LBrac = 62,
     RBrac = 63,
     Assign = 64,
     Range = 65,
     SemiColon = 66,
     Colon = 67,
     Comma = 68,
     NoType = 69
   };
#endif
/* Tokens.  */
#define Boolean 1
#define Integer 2
#define True 3
#define False 4
#define And 10
#define Array 11
#define Begin 12
#define Declare 13
#define Else 14
#define Elsif 15
#define End 16
#define Exit 17
#define For 18
#define If 19
#define In 20
#define Is 21
#define Loop 22
#define Mod 23
#define Not 24
#define On 25
#define Of 26
#define Or 27
#define Procedure 28
#define Then 29
#define When 30
#define While 31
#define Xor 32
#define Ident 40
#define IntConst 41
#define Equal 50
#define DivEq 51
#define LessEq 52
#define GreaterEq 53
#define Less 54
#define Greater 55
#define Plus 56
#define Minus 57
#define Star 58
#define Slash 59
#define LPar 60
#define RPar 61
#define LBrac 62
#define RBrac 63
#define Assign 64
#define Range 65
#define SemiColon 66
#define Colon 67
#define Comma 68
#define NoType 69



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 21 "parser.y"
 tree t; int i; 

/* Line 2058 of yacc.c  */
#line 162 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
