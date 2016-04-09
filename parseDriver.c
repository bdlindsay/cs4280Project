#include "tree.h"

extern FILE *yyin;
extern tree root;
main(int argc, char *argv[]) {
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

	fclose(yyin);
}
