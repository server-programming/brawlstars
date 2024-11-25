all: main.o network_connect.o
	gcc -o server main.o network_connect.o
network_connect.o: network_connect.c
	gcc -g -c network_connect.c
main.o: main.c
	gcc -g -c main.c
clean:
	rm *.o server
