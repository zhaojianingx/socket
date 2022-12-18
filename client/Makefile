client: client.o main.o
	g++ -Wall -o client client.o main.o
	rm *.o
client.o: client.h client.cpp
	g++ -o client.o -c client.cpp
main.o: main.cpp
	g++ -o main.o -c main.cpp
