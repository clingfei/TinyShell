TinyShell: main.o
	cc -o TinyShell main.o
main.o: main.c main.h
	cc -c main.c
clean:
	-rm *.o TinyShell