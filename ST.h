// Brett Lindsay
// Symbol Table Header file
// cs4280 Part 3

typedef enum bool {false, true};

typedef struct {
	char *name;
	int type;
	int scope;
	int upperAlimit;
	int lowerAlimit;
	int arrayBaseT;
	int index; // need this?
	bool valid;	// valid entry in ST right now? 
} STEntry;

extern STEntry *ST[]; // the symbol table
extern STEntry *symStack[]; // the stack for multiple scope support
