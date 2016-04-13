// Brett Lindsay
// Symbol Table Header file
// cs4280 Part 3

typedef enum {false, true} bool;

typedef struct {
	int index; // need this?
	int type;
	char name[30]; 
	int scope;
	int aStart;
	int aEnd;
	int arrayBaseT;
	bool valid;	// valid entry in ST right now? 
} STEntry;

extern STEntry *ST[]; // the symbol table
extern STEntry *symStack[]; // the stack for multiple scope support
