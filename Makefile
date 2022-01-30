TinyShell: main.o
	cc -o TinyShell main.o
main.o: main.c main.h
	cc -c main.c
.PHONY: clean
clean:
	- rm *.o TinyShell