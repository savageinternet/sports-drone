CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv)

all: build/main

build/main:
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o build/main
