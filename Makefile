#Brett Lindsay Makefile
# CS4280 Project
all: ada 
CC = gcc -g
LEX = lex
BISON = bison
BFLAGS = -d -y #-v
OBJS = parseDriver.o tree.o lex.yy.o y.tab.o check.o

ada : $(OBJS)
	$(CC) -o $@ $^

check.o : check.c ST.h
	$(CC) -c check.c

tree.o : tree.c tree.h
	$(CC) -c tree.c

#driver.o : driver.c
#	$(CC) -c driver.c

lex.yy.o : lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c : scanner.l
	$(LEX) scanner.l

y.tab.c : parser.y
	$(BISON) $(BFLAGS) parser.y

y.tab.o : y.tab.c y.tab.h
	$(CC) -c y.tab.c

parseDriver.o : parseDriver.c ST.h 
	$(CC) -c parseDriver.c

clean :
	-@ rm -f lex.yy.c *.o scantest ada y.output *.h.gch y.tab.c
