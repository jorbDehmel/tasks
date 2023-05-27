STEM := clang++ -pedantic -Wall -lboost_system -lboost_filesystem

all:	tasks.out

tasks.out:	tasks.o
	$(STEM) -o tasks.out tasks.o

tasks.o:	tasks.cpp
	$(STEM) -c -o tasks.o tasks.cpp

clean:
	rm -rf *.o *.out
