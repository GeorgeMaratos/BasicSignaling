make: childProcess.c main.c
	gcc -c childProcess.c
	gcc main.c childProcess.o -o signals
