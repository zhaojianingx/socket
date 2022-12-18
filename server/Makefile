server: server.o main.o
	g++ -Wall -o server server.o main.o
	rm *.o
server.o: server.h server.cpp
	g++ -o server.o -c server.cpp
main.o: main.cpp
	g++ -o main.o -c main.cpp
