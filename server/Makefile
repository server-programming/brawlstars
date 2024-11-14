all: main.o
	gcc -o server main.o
main.o: main.c
	gcc -g -c main.c
clean:
	rm *.o server
