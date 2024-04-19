all: main

main: Element.o Card.o IO.o main.o
	g++ IO.o Element.o Card.o main.o -lncurses -lpanel -o main

main.o: main.cpp
	g++ main.cpp -lncurses -lpanel -c -o main.o

Element.o: Element.cpp
	g++ Element.cpp -lncurses -lpanel -c -o Element.o

Card.o: Card.cpp
	g++ Card.cpp -lncurses -c -o Card.o

IO.o: IO.cpp
	g++ IO.cpp -lncurses -lpanel -c -o IO.o
