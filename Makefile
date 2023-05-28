STEM := clang++ -pedantic -Wall
LIBS := -lboost_system -lboost_filesystem
CFLAGS :=

all:	tasks.out

tasks.out:	tasks.o helpers.o
	$(STEM) $(LIBS) -o tasks.out tasks.o helpers.o

tasks.o:	tasks.cpp
	$(STEM) $(CFLAGS) -c -o tasks.o tasks.cpp

helpers.o:	helpers.hpp helpers.cpp
	$(STEM) $(CFLAGS) -c -o helpers.o helpers.cpp

install: all
	sudo cp tasks.out /usr/bin/tasks

uninstall:
	sudo rm -rf ~/.tasks /usr/bin/tasks

clean:
	rm -rf *.o *.out
