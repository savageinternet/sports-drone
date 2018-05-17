CFLAGS=-Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv zbar)

all: build/main

build/main:
	mkdir -p build
	rm -f build/*
	g++ $(CFLAGS) $(LDFLAGS) main.cpp -o build/main

clean:
	rm -r build
