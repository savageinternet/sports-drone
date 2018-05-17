CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv)

all: main

main: main.o
	g++ $(LDFLAGS) -o main main.o

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

clean:
	rm main*
