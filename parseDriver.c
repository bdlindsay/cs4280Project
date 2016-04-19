#include "tree.h"
#include "ST.h"

extern FILE *yyin;
extern tree root;
extern STEntry *ST[100];
extern error_num;
extern yynerrs;

main(int argc, char *argv[]) {
	int i;
	if (argc != 2) {
		printf ("Insufficient args");
		exit(1);
	}
	
	if ((yyin = fopen(argv[1], "r")) == 0L) {
		fprintf(stderr, "%s: can't open input file %s\n", argv[0], argv[1]);
		exit(1);
	}

	yyparse();
	printTree(root);
	
	check_stmts(root); // also creates symbol table
	fprintf(stderr, "Syntax checker found %d errors.\n", yynerrs);	
	fprintf(stderr, "Context checker found %d errors.\n", error_num);	
	/*for ( i = 0; i < 100; i++) { // causing error -> free() invalid next size (fast)
		free( (STEntry*) ST[i] ); // system cleans up malloc'd memory after program termination
  }	*/
	fclose(yyin);
}
