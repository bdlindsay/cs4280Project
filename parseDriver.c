#include "tree.h"
#include "ST.h"
#include "code.h"

extern FILE *yyin;
extern tree root;
extern STEntry *ST[100];
extern error_num;
extern yynerrs;

FILE *outfile;

//extern void gen_program (tree);

main(int argc, char *argv[]) {
	int i;
	if (argc != 3) {
		printf ("Insufficient args");
		exit(1);
	}
	
	if ((yyin = fopen(argv[1], "r")) == 0L) {
		fprintf(stderr, "%s: can't open input file %s\n", argv[0], argv[1]);
		exit(1);
	}

	// part 2
	yyparse();
	printTree(root);
	
	// part 3
	check_stmts(root); // also creates symbol table
	//fprintf(stderr, "Syntax checker found %d errors.\n", yynerrs);	
	//fprintf(stderr, "Context checker found %d errors.\n", error_num);	
	
	// part 4
	if ((outfile = fopen (argv[2], "w")) == 0L) {
		fprintf (stderr, "%s: Can't open output file %s\n", argv[0], argv[2]);
		exit (1);
	}	
	gen_program (root);

	//fclose (outfile);
	fclose (yyin);
}
