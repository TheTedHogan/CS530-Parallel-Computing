# Setup a variable to reference the desired compiler
CC = gcc

# Setup a variable to hold desired compiler flags
CFLAGS = -Wall -Wextra -Wpointer-arith -Wstrict-prototypes -std=c99 -g -lm

# Declaring the build, test and clean rules as phony means that no artifact
#   is created by these rules with a cooresponding name.  Therefore, the
#   rule should run when executed.
.PHONY: build test clean

# The build rule depends upon the myprog artifact existing.  If it does not, make
#   will look for a rule to create it.
build: myprog mytest


myprog: main.o 
	$(CC) $(CFLAGS) $^ -o $@ 


mytest: mytest.o
	$(CC) $(CFLAGS) $^ -o $@ 

test:
	./mytest
clean:
	rm -f *.o *.d a.out myprog mytests main